#pragma once

#include "IntersectionRecord.h"
#include "Sphere.h"
#include "TriangleVertices.h"
#include "math/Camera.h"
#include "math/Ray.h"
#include "math/Vec3.h"
#include "util/ArrayOutput.h"
#include "util/MaterialSpec.h"
#include "util/RenderParams.h"

#include <array>
#include <functional>
#include <optional>
#include <random>
#include <vector>

namespace dod {

class Scene {
  using TriangleNormals = std::array<Norm3, 3>;

  std::vector<TriangleVertices> triangleVerts_;
  std::vector<TriangleNormals> triangleNormals_;
  std::vector<MaterialSpec> triangleMaterials_;

  std::vector<Sphere> spheres_;
  std::vector<MaterialSpec> sphereMaterials_;

  Vec3 environment_;

public:
  [[nodiscard]] Vec3 radiance(std::mt19937 &rng, const Ray &ray, int depth,
                              const RenderParams &renderParams) const;

  void addTriangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
                   const MaterialSpec &material);
  void addSphere(const Vec3 &centre, double radius,
                 const MaterialSpec &material);

  void setEnvironmentColour(const Vec3 &colour);

  ArrayOutput
  render(const Camera &camera, const RenderParams &renderParams,
         const std::function<void(ArrayOutput &output)> &updateFunc);

  // Visible for tests
  [[nodiscard]] std::optional<IntersectionRecord>
  intersectSpheres(const Ray &ray, double nearerThan) const;

  [[nodiscard]] std::optional<IntersectionRecord>
  intersectTriangles(const Ray &ray, double nearerThan) const;

  [[nodiscard]] std::optional<dod::IntersectionRecord>
  intersect(const Ray &ray) const;
};

}
