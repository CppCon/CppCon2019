#include "ArrayOutput.h"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <numeric>

namespace {
std::uint8_t componentToInt(double x) {
  return static_cast<uint8_t>(
      lround(pow(std::clamp(x, 0.0, 1.0), 1.0 / 2.2) * 255));
}

struct FileCloser {
  void operator()(FILE *f) {
    if (f)
      fclose(f);
  }
};

struct Header {
  static constexpr uint32_t Signature = 1;
  static constexpr uint32_t Version = 1;
  uint32_t signature{Signature};
  uint32_t version{Version};
  uint32_t height{};
  uint32_t width{};
};

}

ArrayOutput::Pixel ArrayOutput::pixelAt(int x, int y) const noexcept {
  auto rawPixel = rawPixelAt(x, y);
  return ArrayOutput::Pixel{componentToInt(rawPixel.x()),
                            componentToInt(rawPixel.y()),
                            componentToInt(rawPixel.z())};
}

void ArrayOutput::addSamples(int x, int y, const Vec3 &colour,
                             int numSamples) noexcept {
  output_[indexOf(x, y)].accumulate(colour, numSamples);
}

Vec3 ArrayOutput::rawPixelAt(int x, int y) const noexcept {
  return output_[indexOf(x, y)].result();
}

ArrayOutput &ArrayOutput::operator+=(const ArrayOutput &rhs) {
  if (rhs.width() != width() || rhs.height() != height())
    throw std::logic_error(
        "Two differently-sized arrays were attempted to be combined");
  for (size_t pixelIndex = 0; pixelIndex < output_.size(); ++pixelIndex) {
    output_[pixelIndex].accumulate(rhs.output_[pixelIndex]);
  }
  return *this;
}

size_t ArrayOutput::totalSamples() const noexcept {
  return std::accumulate(begin(output_), end(output_), 0ULL,
                         [](size_t lhs, const SampledPixel &pixel) {
                           return lhs + pixel.numSamples();
                         });
}

void ArrayOutput::save(const std::string &filename) const {
  std::unique_ptr<FILE, FileCloser> out(fopen(filename.c_str(), "wb"));
  if (!out)
    throw std::runtime_error("Unable to open " + filename);
  Header header{Header::Signature, Header::Version,
                static_cast<uint32_t>(height_), static_cast<uint32_t>(width_)};
  auto W = [&](const auto &object) {
    if (fwrite(&object, sizeof(object), 1, out.get()) != 1) {
      throw std::runtime_error("Unable to write to " + filename);
    }
  };
  W(header);
  for (auto &&pixel : output_) {
    W(pixel.rawResult());
    W(static_cast<uint32_t>(pixel.numSamples()));
  }
}

ArrayOutput ArrayOutput::load(const std::string &filename) {
  std::unique_ptr<FILE, FileCloser> in(fopen(filename.c_str(), "rb"));
  if (!in)
    throw std::runtime_error("Unable to open " + filename);
  auto R = [&](auto &object) {
    if (fread(&object, sizeof(object), 1, in.get()) != 1) {
      throw std::runtime_error("Unable to read from " + filename);
    }
  };
  Header header;
  R(header);
  if (header.signature != Header::Signature)
    throw std::runtime_error("Bad file " + filename + " : bad signature");
  if (header.version != Header::Version)
    throw std::runtime_error("Bad file " + filename + " : bad version");
  ArrayOutput result(header.width, header.height);
  for (int y = 0; y < result.height(); ++y) {
    for (int x = 0; x < result.width(); ++x) {
      Vec3 vec;
      uint32_t s;
      R(vec);
      R(s);
      result.addSamples(x, y, vec, s);
    }
  }

  return result;
}
