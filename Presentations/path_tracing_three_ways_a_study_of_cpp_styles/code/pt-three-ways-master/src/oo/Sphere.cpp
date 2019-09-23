#include "Sphere.h"
#include "math/Epsilon.h"

using oo::Sphere;

bool Sphere::intersect(const Ray &ray, Hit &hit) const noexcept {
  // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
  auto op = centre_ - ray.origin();
  auto radiusSquared = radius_ * radius_;
  auto b = op.dot(ray.direction().toVec3());
  auto determinant = b * b - op.lengthSquared() + radiusSquared;
  if (determinant < 0)
    return false;

  determinant = sqrt(determinant);
  auto minusT = b - determinant;
  auto plusT = b + determinant;
  if (minusT < Epsilon && plusT < Epsilon)
    return false;

  auto t = minusT > Epsilon ? minusT : plusT;
  auto hitPosition = ray.positionAlong(t);
  auto normal = (hitPosition - centre_).normalised();
  bool inside = normal.dot(ray.direction()) > 0;
  if (inside)
    normal = -normal;
  hit = Hit{t, inside, hitPosition, normal};
  return true;
}
