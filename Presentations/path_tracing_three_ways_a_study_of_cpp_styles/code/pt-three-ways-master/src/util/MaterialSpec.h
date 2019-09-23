#pragma once

#include "math/Vec3.h"

#include <cmath>

struct MaterialSpec {
  Vec3 emission;
  Vec3 diffuse;
  double indexOfRefraction{1.0};
  double reflectivity{-1};
  double reflectionConeAngleRadians{0.0};
  static double toRadians(double angle) { return angle / 360 * 2 * M_PI; }
  static MaterialSpec makeDiffuse(const Vec3 &colour) {
    return MaterialSpec{Vec3(), colour};
  }
  static MaterialSpec makeSpecular(const Vec3 &colour, double index) {
    return MaterialSpec{Vec3(), colour, index};
  }
  static MaterialSpec makeLight(const Vec3 &colour) {
    return MaterialSpec{colour, Vec3()};
  }
  static MaterialSpec makeGlossy(const Vec3 &colour, double index,
                                 double reflectionConeAngleDegrees) {
    return MaterialSpec{Vec3(), colour, index, -1,
                        toRadians(reflectionConeAngleDegrees)};
  }
  static MaterialSpec makeReflective(const Vec3 &colour, double reflectivity,
                                     double reflectionConeAngleDegrees) {
    return MaterialSpec{Vec3(), colour, 1.0, reflectivity,
                        toRadians(reflectionConeAngleDegrees)};
  }
  bool operator==(const MaterialSpec &rhs) const {
    return emission == rhs.emission && diffuse == rhs.diffuse
           && indexOfRefraction == rhs.indexOfRefraction
           && reflectivity == rhs.reflectivity
           && reflectionConeAngleRadians == rhs.reflectionConeAngleRadians;
  }
  bool operator!=(const MaterialSpec &rhs) const { return !(rhs == *this); }
};
