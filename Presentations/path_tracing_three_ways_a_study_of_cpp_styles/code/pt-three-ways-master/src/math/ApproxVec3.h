#pragma once

#include "Norm3.h"
#include "Vec3.h"

#include <iosfwd>

struct ApproxVec3 {
  Vec3 vec_;
  double epsilon_ = 0.0001;

public:
  ApproxVec3(double x, double y, double z) : vec_(x, y, z) {}
  explicit ApproxVec3(const Vec3 &vec) : vec_(vec.x(), vec.y(), vec.z()) {}
  explicit ApproxVec3(const Norm3 &vec) : vec_(vec.x(), vec.y(), vec.z()) {}
  friend constexpr bool operator==(const ApproxVec3 &lhs,
                                   const Vec3 &rhs) noexcept {
    return (lhs.vec_ - rhs).lengthSquared() < (lhs.epsilon_ * lhs.epsilon_);
  }
  friend constexpr bool operator==(const Vec3 &lhs,
                                   const ApproxVec3 &rhs) noexcept {
    return (lhs - rhs.vec_).lengthSquared() < (rhs.epsilon_ * rhs.epsilon_);
  }
  friend constexpr bool operator==(const Norm3 &lhs,
                                   const ApproxVec3 &rhs) noexcept {
    return (lhs.toVec3() - rhs.vec_).lengthSquared()
           < (rhs.epsilon_ * rhs.epsilon_);
  }
  friend constexpr bool operator!=(const ApproxVec3 &lhs,
                                   const Vec3 &rhs) noexcept {
    return !(lhs == rhs);
  }
  friend constexpr bool operator!=(const Vec3 &lhs,
                                   const ApproxVec3 &rhs) noexcept {
    return !(lhs == rhs);
  }
  friend constexpr bool operator!=(const Norm3 &lhs,
                                   const ApproxVec3 &rhs) noexcept {
    return !(lhs == rhs);
  }
  friend std::ostream &operator<<(std::ostream &, const ApproxVec3 &);
};