#include "SceneBuilder.h"
#include "Primitive.h"
#include "Sphere.h"
#include "Triangle.h"

using oo::SceneBuilder;

namespace oo {
namespace {

struct SpherePrimitive : Primitive {
  Sphere sphere;
  std::unique_ptr<Material> material;
  SpherePrimitive(const Sphere &sphere, std::unique_ptr<Material> material)
      : sphere(sphere), material(std::move(material)) {}
  [[nodiscard]] bool intersect(const Ray &ray,
                               IntersectionRecord &rec) const override {
    Hit hit;
    if (!sphere.intersect(ray, hit))
      return false;
    rec = IntersectionRecord{hit, material.get()};
    return true;
  }
};

struct TrianglePrimitive : Primitive {
  Triangle triangle;
  std::unique_ptr<Material> material;
  explicit TrianglePrimitive(const Triangle &triangle,
                             std::unique_ptr<Material> material)
      : triangle(triangle), material(std::move(material)) {}
  [[nodiscard]] bool
  intersect(const Ray &ray,
            IntersectionRecord &intersectionRecord) const override {
    Hit hit;
    if (!triangle.intersect(ray, hit))
      return false;
    intersectionRecord = IntersectionRecord{hit, material.get()};
    return true;
  }
};

}
}

void SceneBuilder::addTriangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
                               const MaterialSpec &material) {
  scene_.add(std::make_unique<TrianglePrimitive>(Triangle(v0, v1, v2),
                                                 Material::from(material)));
}
void SceneBuilder::addSphere(const Vec3 &centre, double radius,
                             const MaterialSpec &material) {
  scene_.add(std::make_unique<SpherePrimitive>(Sphere(centre, radius),
                                               Material::from(material)));
}

void SceneBuilder::setEnvironmentColour(const Vec3 &colour) {
  scene_.setEnvironmentColour(colour);
}
