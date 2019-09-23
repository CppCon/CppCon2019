#pragma once

#include "OrthoNormalBasis.h"
#include "Ray.h"
#include "Vec3.h"

#include <cmath>
#include <random>

class Camera {
  Vec3 centre_;
  OrthoNormalBasis axis_;
  double aspectRatio_;
  double cameraPlaneDist_;
  double reciprocalHeight_;
  double reciprocalWidth_;
  double apertureRadius_{};
  double focalDistance_{};

  template <typename Rng>
  [[nodiscard]] Ray rayFromUnit(double x, double y, Rng &rng) const {
    auto xContrib = axis_.x() * -x * aspectRatio_;
    auto yContrib = axis_.y() * -y;
    auto zContrib = axis_.z() * cameraPlaneDist_;
    auto direction = (xContrib + yContrib + zContrib).normalised();
    if (apertureRadius_ == 0)
      return Ray(centre_, direction);

    auto focalPoint = centre_ + direction * focalDistance_;
    std::uniform_real_distribution<> angleDist(0, 2 * M_PI);
    std::uniform_real_distribution<> radiusDist(0, apertureRadius_);
    auto angle = angleDist(rng);
    auto radius = radiusDist(rng);
    auto origin = centre_ + (axis_.x() * cos(angle) * radius)
                  + (axis_.y() * sin(angle) * radius);
    return Ray::fromTwoPoints(origin, focalPoint);
  }

public:
  Camera(const Vec3 &eye, const Vec3 &lookAt, const Norm3 &up, int width,
         int height, double verticalFov)
      : centre_(eye),
        axis_(OrthoNormalBasis::fromZY((lookAt - eye).normalised(), up)),
        aspectRatio_(static_cast<double>(width) / height),
        cameraPlaneDist_(1.0 / tan(verticalFov * M_PI / 360.0)),
        reciprocalHeight_(1.0 / height), reciprocalWidth_(1.0 / width) {}

  void setFocus(const Vec3 &focalPoint, double apertureRadius) {
    focalDistance_ = (focalPoint - centre_).length();
    apertureRadius_ = apertureRadius;
  }

  // The pixel center is at 0.5, 0.5 within a pixel.
  template <typename Rng>
  [[nodiscard]] Ray randomRay(int pixelX, int pixelY, Rng &rng) const {
    std::uniform_real_distribution<> unit;
    auto x = (pixelX + unit(rng)) * reciprocalWidth_;
    auto y = (pixelY + unit(rng)) * reciprocalHeight_;
    return rayFromUnit(2 * x - 1, 2 * y - 1, rng);
  }
};