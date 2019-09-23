#pragma once

#include "math/Hit.h"
#include "math/Ray.h"
#include "math/Vec3.h"
#include "optional.hpp"

#include <array>

namespace fp {

class Triangle {
public:
  using Vertices = std::array<Vec3, 3>;
  using Normals = std::array<Norm3, 3>;

private:
  Vertices vertices_;
  Normals normals_;

public:
  explicit Triangle(const Vertices &vertices);
  Triangle(const Vertices &vertices, const Normals &normals)
      : vertices_(vertices), normals_(normals) {}
  Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
      : Triangle(Vertices{v0, v1, v2}) {}

  [[nodiscard]] constexpr const Vec3 &vertex(int index) const {
    return vertices_[index];
  }

  [[nodiscard]] constexpr const Norm3 &normal(int index) const {
    return normals_[index];
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
  [[nodiscard]] tl::optional<Hit> intersect(const Ray &ray) const noexcept;
};

}