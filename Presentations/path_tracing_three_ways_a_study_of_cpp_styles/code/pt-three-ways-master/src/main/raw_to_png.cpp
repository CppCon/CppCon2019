#include "PngWriter.h"
#include "util/ArrayOutput.h"

#include <clara.hpp>

#include <iomanip>
#include <iostream>

int main(int argc, const char *argv[]) {
  using namespace clara;

  bool help = false;
  std::string outputName;
  std::vector<std::string> inputs;

  auto cli = Arg(outputName, "output")("output filename").required()
             | Arg(inputs, "input")("input filename").required() | Help(help);

  auto result = cli.parse(Args(argc, argv));
  if (!result) {
    std::cerr << "Error in command line: " << result.errorMessage() << '\n';
    exit(1);
  }
  if (help) {
    std::cout << cli;
    exit(0);
  }

  if (outputName.empty()) { // https://github.com/catchorg/Clara/issues/39
    std::cerr << "Missing output filename.\n" << cli;
    exit(1);
  }

  if (inputs.empty()) { // https://github.com/catchorg/Clara/issues/39
    std::cerr << "Missing inputs.\n" << cli;
    exit(1);
  }

  std::optional<ArrayOutput> accumulator;
  size_t totalSamples{};
  for (auto &inputFilename : inputs) {
    std::cout << "Loading " << inputFilename << "...\n";
    auto input = ArrayOutput::load(inputFilename);
    if (!accumulator) {
      std::cout << "  width: " << input.width() << " height: " << input.height()
                << '\n';
      accumulator.emplace(input.width(), input.height());
    }
    auto samples = input.totalSamples();
    totalSamples += samples;
    std::cout << "  samples: " << samples << '\n';
    if (accumulator->width() != input.width()
        || accumulator->height() != input.height()) {
      std::cerr << "Mismatch in size, width " << input.width() << " height "
                << input.height() << '\n';
      exit(1);
    }
    *accumulator += input;
  }
  auto averageSpp = static_cast<double>(totalSamples)
                    / (accumulator->width() * accumulator->height());
  std::cout << "Saving " << outputName << " with " << totalSamples
            << " samples (" << std::fixed << std::setprecision(1) << averageSpp
            << " per pixel)"
            << "...\n";
  PngWriter pw(outputName.c_str(), accumulator->width(), accumulator->height());
  if (!pw.ok()) {
    std::cerr << "Unable to save PNG\n";
    exit(1);
  }

  for (int y = 0; y < accumulator->height(); ++y) {
    std::uint8_t row[accumulator->width() * 3];
    for (int x = 0; x < accumulator->width(); ++x) {
      auto colour = accumulator->pixelAt(x, y);
      for (int component = 0; component < 3; ++component)
        row[x * 3 + component] = colour[component];
    }
    pw.addRow(row);
  }
}