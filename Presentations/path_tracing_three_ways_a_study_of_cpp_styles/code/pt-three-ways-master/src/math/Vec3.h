#pragma once

#include <cmath>
#include <iosfwd>

class Norm3;

class Vec3 {
  double x_{}, y_{}, z_{};

public:
  constexpr Vec3() noexcept = default;
  constexpr Vec3(double x, double y, double z) noexcept : x_(x), y_(y), z_(z) {}
  constexpr explicit Vec3(const Norm3 &norm);

  constexpr Vec3 operator+(const Vec3 &b) const noexcept {
    return Vec3(x_ + b.x_, y_ + b.y_, z_ + b.z_);
  }
  constexpr Vec3 &operator+=(const Vec3 &b) noexcept {
    x_ += b.x_;
    y_ += b.y_;
    z_ += b.z_;
    return *this;
  }

  constexpr Vec3 operator-(const Vec3 &b) const noexcept {
    return Vec3(x_ - b.x_, y_ - b.y_, z_ - b.z_);
  }
  constexpr Vec3 &operator-=(const Vec3 &b) noexcept {
    x_ -= b.x_;
    y_ -= b.y_;
    z_ -= b.z_;
    return *this;
  }

  friend Vec3 operator*(double lhs, const Vec3 &rhs) {
    return Vec3(lhs * rhs.x_, lhs * rhs.y_, lhs * rhs.z_);
  }
  constexpr Vec3 operator*(double b) const noexcept {
    return Vec3(x_ * b, y_ * b, z_ * b);
  }
  constexpr Vec3 &operator*=(double b) noexcept {
    x_ *= b;
    y_ *= b;
    z_ *= b;
    return *this;
  }
  friend Vec3 operator/(double lhs, const Vec3 &rhs) {
    return Vec3(lhs / rhs.x_, lhs / rhs.y_, lhs / rhs.z_);
  }
  constexpr Vec3 operator/(double b) const noexcept {
    const auto reciprocal = 1.0 / b;
    return Vec3(x_ * reciprocal, y_ * reciprocal, z_ * reciprocal);
  }
  constexpr Vec3 &operator/=(double b) noexcept {
    const auto reciprocal = 1.0 / b;
    x_ *= reciprocal;
    y_ *= reciprocal;
    z_ *= reciprocal;
    return *this;
  }

  constexpr Vec3 operator*(const Vec3 &b) const noexcept {
    return Vec3(x_ * b.x_, y_ * b.y_, z_ * b.z_);
  }
  constexpr Vec3 &operator*=(const Vec3 &b) noexcept {
    x_ *= b.x_;
    y_ *= b.y_;
    z_ *= b.z_;
    return *this;
  }

  constexpr Vec3 operator-() const noexcept { return Vec3(-x_, -y_, -z_); }

  [[nodiscard]] constexpr double lengthSquared() const noexcept {
    return dot(*this);
  }
  [[nodiscard]] double length() const noexcept { return sqrt(lengthSquared()); }

  [[nodiscard]] Norm3 normalised() const noexcept;

  [[nodiscard]] constexpr double dot(const Vec3 &b) const noexcept {
    return x_ * b.x_ + y_ * b.y_ + z_ * b.z_;
  }

  [[nodiscard]] constexpr Vec3 cross(const Vec3 &b) const noexcept {
    auto x = y_ * b.z_ - z_ * b.y_;
    auto y = z_ * b.x_ - x_ * b.z_;
    auto z = x_ * b.y_ - y_ * b.x_;
    return Vec3(x, y, z);
  }

  constexpr bool operator==(const Vec3 &b) const noexcept {
    return x_ == b.x_ && y_ == b.y_ && z_ == b.z_;
  }
  constexpr bool operator!=(const Vec3 &b) const noexcept {
    return x_ != b.x_ || y_ != b.y_ || z_ != b.z_;
  }

  [[nodiscard]] constexpr double x() const noexcept { return x_; }
  [[nodiscard]] constexpr double y() const noexcept { return y_; }
  [[nodiscard]] constexpr double z() const noexcept { return z_; }

  [[nodiscard]] static constexpr Vec3 xAxis() { return Vec3(1, 0, 0); }
  [[nodiscard]] static constexpr Vec3 yAxis() { return Vec3(0, 1, 0); }
  [[nodiscard]] static constexpr Vec3 zAxis() { return Vec3(0, 0, 1); }
};

std::ostream &operator<<(std::ostream &o, const Vec3 &v);

#include "Vec3.impl.h"