#pragma once

#include "MaterialSpec.h"

#include <iosfwd>
#include <memory>
#include <string>

struct ObjLoaderOpener {
  virtual ~ObjLoaderOpener() = default;
  virtual std::unique_ptr<std::istream> open(const std::string &filename) = 0;
};

template <typename SceneBuilder>
void loadObjFile(std::istream &in, ObjLoaderOpener &opener, SceneBuilder &sb);

#include "ObjLoaderImpl.h"
