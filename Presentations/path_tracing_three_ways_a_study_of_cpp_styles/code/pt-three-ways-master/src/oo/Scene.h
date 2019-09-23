#pragma once

#include "oo/Primitive.h"

#include <memory>
#include <vector>

namespace oo {

class Scene : public Primitive {
  std::vector<std::unique_ptr<Primitive>> primitives_;
  Vec3 environment_;

public:
  void setEnvironmentColour(const Vec3 &colour) { environment_ = colour; }
  void add(std::unique_ptr<Primitive> primitive);

  [[nodiscard]] bool intersect(const Ray &ray,
                               IntersectionRecord &intersection) const override;
  [[nodiscard]] Vec3 environment(const Ray &ray) const;
};

}