#pragma once

#include <cstddef>

class Progressifier {
  size_t numWork_{};
  double minProgress_{5.0};
  double lastProgress_{};

public:
  explicit Progressifier(size_t numWork) noexcept;

  void update(size_t numDone) noexcept;
  void numLeft(size_t numLeft) noexcept { update(numWork_ - numLeft); }
};
