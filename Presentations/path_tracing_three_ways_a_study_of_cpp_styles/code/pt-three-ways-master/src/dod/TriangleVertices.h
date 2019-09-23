#pragma once

#include "math/Vec3.h"
#include "util/MaterialSpec.h"
#include <array>

namespace dod {

class TriangleVertices {
public:
  using Vertices = std::array<Vec3, 3>;

private:
  Vertices vertices_;

public:
  explicit TriangleVertices(const Vertices &vertices) : vertices_(vertices) {}
  TriangleVertices(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
      : TriangleVertices(Vertices{v0, v1, v2}) {}

  [[nodiscard]] constexpr const Vec3 &vertex(int index) const {
    return vertices_[index];
  }

  [[nodiscard]] constexpr Vec3 uVector() const {
    return vertices_[1] - vertices_[0];
  }

  [[nodiscard]] constexpr Vec3 vVector() const {
    return vertices_[2] - vertices_[0];
  }

  [[nodiscard]] Norm3 faceNormal() const {
    return uVector().cross(vVector()).normalised();
  }
};

}