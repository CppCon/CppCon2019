#include "Triangle.h"
#include "math/Epsilon.h"
#include "optional.hpp"
#include "util/Unpredictable.h"

using fp::Triangle;

// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
tl::optional<Hit> Triangle::intersect(const Ray &ray) const noexcept {
  const auto pVec = ray.direction().cross(vVector());
  const auto det = uVector().dot(pVec);

  // ray and triangle are parallel if det is close to 0
  if (fabs(det) < Epsilon)
    return {};

  const auto backfacing = det < Epsilon;

  const auto invDet = 1.0 / det;
  const auto tVec = ray.origin() - vertices_[0];
  const auto u = tVec.dot(pVec) * invDet;
  const auto qVec = tVec.cross(uVector());
  const auto v = ray.direction().dot(qVec) * invDet;

  // extra parens to keep clang-format happy...
  if (Unpredictable::any((u) < 0.0, u > 1.0, (v) < 0.0, u + v > 1.0))
    return {};

  const auto t = vVector().dot(qVec) * invDet;

  if (t < Epsilon)
    return {};

  const auto normalUdelta = normals_[1].toVec3() - normals_[0].toVec3();
  const auto normalVdelta = normals_[2].toVec3() - normals_[0].toVec3();
  // TODO: proper barycentric coordinates
  const auto normal =
      ((u * normalUdelta) + (v * normalVdelta) + normals_[0].toVec3())
          .normalised();
  return Hit{t, backfacing, ray.positionAlong(t),
             backfacing ? -normal : normal};
}

Triangle::Triangle(const Triangle::Vertices &vertices)
    : vertices_(vertices), normals_{Norm3::xAxis(), Norm3::xAxis(),
                                    Norm3::xAxis()} {
  normals_[0] = normals_[1] = normals_[2] = faceNormal();
}
