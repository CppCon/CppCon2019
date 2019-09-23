#pragma once

#include <iosfwd>

class Vec3;

class Norm3 {
  double x_{}, y_{}, z_{};

  friend class Vec3;
  constexpr explicit Norm3(const Vec3 &vec) noexcept;
  constexpr explicit Norm3(double x, double y, double z) noexcept
      : x_(x), y_(y), z_(z) {}

public:
  [[nodiscard]] constexpr Vec3 toVec3() const noexcept;

  static Norm3 fromNormal(const Vec3 &normal);

  constexpr bool operator==(const Norm3 &b) const noexcept {
    return x_ == b.x_ && y_ == b.y_ && z_ == b.z_;
  }
  constexpr bool operator!=(const Norm3 &b) const noexcept {
    return x_ != b.x_ || y_ != b.y_ || z_ != b.z_;
  }

  [[nodiscard]] constexpr Norm3 operator-() const noexcept {
    return Norm3(-x_, -y_, -z_);
  }

  [[nodiscard]] constexpr Norm3 reflect(const Norm3 &incoming) const noexcept;
  [[nodiscard]] double reflectance(const Norm3 &incoming, double iorFrom,
                                   double iorTo) const noexcept;

  [[nodiscard]] constexpr double dot(const Vec3 &b) const noexcept;
  [[nodiscard]] constexpr double dot(const Norm3 &b) const noexcept;
  [[nodiscard]] constexpr Vec3 cross(const Vec3 &b) const noexcept;
  [[nodiscard]] constexpr Vec3 cross(const Norm3 &b) const noexcept;

  [[nodiscard]] constexpr Vec3 operator*(double b) const noexcept;

  [[nodiscard]] constexpr double x() const noexcept { return x_; }
  [[nodiscard]] constexpr double y() const noexcept { return y_; }
  [[nodiscard]] constexpr double z() const noexcept { return z_; }

  [[nodiscard]] static constexpr Norm3 xAxis() { return Norm3(1, 0, 0); }
  [[nodiscard]] static constexpr Norm3 yAxis() { return Norm3(0, 1, 0); }
  [[nodiscard]] static constexpr Norm3 zAxis() { return Norm3(0, 0, 1); }
};

std::ostream &operator<<(std::ostream &o, const Norm3 &v);

#include "Norm3.impl.h"
