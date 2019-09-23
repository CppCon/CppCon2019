#include "OrthoNormalBasis.h"
#include <cstdio>

OrthoNormalBasis OrthoNormalBasis::fromXY(const Norm3 &x, const Norm3 &y) {
  auto zz = x.cross(y).normalised();
  auto yy = Norm3::fromNormal(zz.cross(x));
  return OrthoNormalBasis(x, yy, zz);
}

OrthoNormalBasis OrthoNormalBasis::fromYX(const Norm3 &y, const Norm3 &x) {
  auto zz = x.cross(y).normalised();
  auto xx = Norm3::fromNormal(y.cross(zz));
  return OrthoNormalBasis(xx, y, zz);
}

OrthoNormalBasis OrthoNormalBasis::fromXZ(const Norm3 &x, const Norm3 &z) {
  auto yy = z.cross(x).normalised();
  auto zz = Norm3::fromNormal(x.cross(yy));
  return OrthoNormalBasis(x, yy, zz);
}

OrthoNormalBasis OrthoNormalBasis::fromZX(const Norm3 &z, const Norm3 &x) {
  auto yy = z.cross(x).normalised();
  auto xx = Norm3::fromNormal(yy.cross(z));
  return OrthoNormalBasis(xx, yy, z);
}

OrthoNormalBasis OrthoNormalBasis::fromYZ(const Norm3 &y, const Norm3 &z) {
  auto xx = y.cross(z).normalised();
  auto zz = Norm3::fromNormal(xx.cross(y));
  return OrthoNormalBasis(xx, y, zz);
}

OrthoNormalBasis OrthoNormalBasis::fromZY(const Norm3 &z, const Norm3 &y) {
  auto xx = y.cross(z).normalised();
  auto yy = Norm3::fromNormal(z.cross(xx));
  return OrthoNormalBasis(xx, yy, z);
}

namespace {
const double Coincident = 0.9999;
}

OrthoNormalBasis OrthoNormalBasis::fromZ(const Norm3 &z) {
  auto xx = (fabs(z.dot(Norm3::xAxis())) > Coincident ? Norm3::yAxis()
                                                      : Norm3::xAxis())
                .cross(z)
                .normalised();
  auto yy = z.cross(xx).normalised();
  return OrthoNormalBasis(xx, yy, z);
}
