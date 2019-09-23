#pragma once

#include "math/Vec3.h"

class SampledPixel {
  Vec3 colour_;
  size_t numSamples_{};

public:
  void accumulate(const SampledPixel &sample) noexcept;
  void accumulate(const Vec3 &sample, int num) noexcept;
  [[nodiscard]] Vec3 result() const noexcept;
  [[nodiscard]] Vec3 rawResult() const noexcept { return colour_; }
  [[nodiscard]] constexpr size_t numSamples() const noexcept {
    return numSamples_;
  }
};
