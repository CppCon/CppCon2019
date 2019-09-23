#include "Triangle.h"
#include "math/Epsilon.h"
#include "util/Unpredictable.h"

using oo::Triangle;

// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
bool Triangle::intersect(const Ray &ray, Hit &hit) const noexcept {
  auto pVec = ray.direction().cross(vVector());
  auto det = uVector().dot(pVec);
  // ray and triangle are parallel if det is close to 0
  if (fabs(det) < Epsilon)
    return false;

  auto backfacing = det < Epsilon;

  auto invDet = 1.0 / det;
  auto tVec = ray.origin() - vertices_[0];
  auto u = tVec.dot(pVec) * invDet;

  auto qVec = tVec.cross(uVector());
  auto v = ray.direction().dot(qVec) * invDet;

  // extra parens to keep clang-format happy...
  if (Unpredictable::any((u) < 0.0, u > 1.0, (v) < 0.0, u + v > 1.0))
    return false;

  auto t = vVector().dot(qVec) * invDet;
  if (t < Epsilon)
    return false;

  auto normalUdelta = normals_[1].toVec3() - normals_[0].toVec3();
  auto normalVdelta = normals_[2].toVec3() - normals_[0].toVec3();
  // TODO: proper barycentric coordinates
  auto normal = ((u * normalUdelta) + (v * normalVdelta) + normals_[0].toVec3())
                    .normalised();
  if (backfacing)
    normal = -normal;
  hit = Hit{t, backfacing, ray.positionAlong(t), normal};
  return true;
}

Triangle::Triangle(const Triangle::Vertices &vertices)
    : vertices_(vertices), normals_{Norm3::xAxis(), Norm3::xAxis(),
                                    Norm3::xAxis()} {
  normals_[0] = normals_[1] = normals_[2] = faceNormal();
}
