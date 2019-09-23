#include "SceneBuilder.h"

using fp::SceneBuilder;

void SceneBuilder::addTriangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
                               const MaterialSpec &material) {
  scene_.primitives.emplace_back(
      TrianglePrimitive{Triangle(v0, v1, v2), material});
}

void SceneBuilder::addSphere(const Vec3 &centre, double radius,
                             const MaterialSpec &material) {
  scene_.primitives.emplace_back(
      SpherePrimitive{Sphere(centre, radius), material});
}

void SceneBuilder::setEnvironmentColour(const Vec3 &colour) {
  scene_.environment = colour;
}
