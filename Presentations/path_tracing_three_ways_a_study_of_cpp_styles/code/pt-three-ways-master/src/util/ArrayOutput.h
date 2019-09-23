#pragma once

#include "SampledPixel.h"

#include <array>
#include <stdexcept>
#include <vector>

class ArrayOutput {
  const int width_;
  const int height_;
  std::vector<SampledPixel> output_;

  [[nodiscard]] constexpr int indexOf(int x, int y) const noexcept {
    // TODO "assert" in range?
    return x + y * width_;
  }

public:
  ArrayOutput(int width, int height) : width_(width), height_(height) {
    output_.resize(width * height);
  }

  template <typename Source>
  ArrayOutput(int width, int height, Source &&source)
      : width_(width), height_(height) {
    output_.resize(width * height);
    int index = 0;
    for (auto &&sample : source) {
      if (index >= width * height)
        throw std::logic_error("Too many samples in input");
      output_[index++].accumulate(sample, 1);
    }
    if (index != width * height)
      throw std::logic_error("Too few samples in input");
  }

  [[nodiscard]] constexpr int height() const noexcept { return height_; }
  [[nodiscard]] constexpr int width() const noexcept { return width_; }

  void addSamples(int x, int y, const Vec3 &colour, int numSamples) noexcept;

  [[nodiscard]] Vec3 rawPixelAt(int x, int y) const noexcept;

  using Pixel = std::array<uint8_t, 3>;
  [[nodiscard]] Pixel pixelAt(int x, int y) const noexcept;

  ArrayOutput &operator+=(const ArrayOutput &rhs);

  [[nodiscard]] size_t totalSamples() const noexcept;

  void save(const std::string &filename) const;
  [[nodiscard]] static ArrayOutput load(const std::string &filename);
};
