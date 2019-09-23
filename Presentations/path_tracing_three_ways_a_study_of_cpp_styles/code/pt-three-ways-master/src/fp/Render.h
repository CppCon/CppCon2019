#pragma once

#include "Scene.h"
#include "math/Camera.h"
#include "util/ArrayOutput.h"
#include "util/RenderParams.h"
#include <functional>

namespace fp {

ArrayOutput render(const Camera &camera, const Scene &scene,
                   const RenderParams &renderParams,
                   const std::function<void(const ArrayOutput &)> &updateFunc);

}