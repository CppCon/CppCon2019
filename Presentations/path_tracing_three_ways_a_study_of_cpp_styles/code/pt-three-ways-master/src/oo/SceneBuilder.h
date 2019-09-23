#pragma once

#include "Scene.h"
#include "util/MaterialSpec.h"

namespace oo {

class SceneBuilder {
  Scene scene_;

public:
  void addTriangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
                   const MaterialSpec &material);
  void addSphere(const Vec3 &centre, double radius,
                 const MaterialSpec &material);

  void setEnvironmentColour(const Vec3 &colour);

  [[nodiscard]] const Scene &scene() const { return scene_; }
};

}