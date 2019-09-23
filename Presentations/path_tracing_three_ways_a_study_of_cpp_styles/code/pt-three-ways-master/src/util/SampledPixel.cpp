#include "SampledPixel.h"

void SampledPixel::accumulate(const Vec3 &sample, int num) noexcept {
  colour_ += sample;
  numSamples_ += num;
}

Vec3 SampledPixel::result() const noexcept {
  if (numSamples_ == 0)
    return colour_;
  return colour_ * (1.0 / numSamples_);
}

void SampledPixel::accumulate(const SampledPixel &sample) noexcept {
  colour_ += sample.colour_;
  numSamples_ += sample.numSamples_;
}
