#pragma once

#include "Scene.h"
#include "math/Camera.h"
#include "util/ArrayOutput.h"
#include "util/RenderParams.h"

#include <functional>
#include <random>

namespace oo {

class Renderer {
  const Scene &scene_;
  const Camera &camera_;
  const RenderParams &renderParams_;

public:
  Renderer(const Scene &scene, const Camera &camera,
           const RenderParams &renderParams)
      : scene_(scene), camera_(camera), renderParams_(renderParams) {}

  ArrayOutput
  renderTiled(std::function<void(const ArrayOutput &)> updateFunc) const;
  ArrayOutput
  render(const std::function<void(const ArrayOutput &)> &updateFunc) const;

  // Visible for testing
  struct Tile {
    int xBegin;
    int xEnd;
    int yBegin;
    int yEnd;
    int samples;
    int sampleNum;
    size_t distancePrio;
    size_t randomPrio;
    [[nodiscard]] constexpr auto key() const {
      return std::tie(sampleNum, distancePrio, randomPrio);
    }
  };

  Vec3 radiance(std::mt19937 &rng, const Ray &ray, int depth) const;
  [[nodiscard]] std::vector<Renderer::Tile>
  generateTiles(int xTileSize, int yTileSize, int numSamples,
                int samplesPerTile, int seed) const;
  [[nodiscard]] static std::vector<Tile>
  generateTiles(int width, int height, int xTileSize, int yTileSize,
                int numSamples, int samplesPerTile, int seed);

private:
  class Sampler;
};

}
