#include "Renderer.h"
#include "util/WorkQueue.h"

#include <future>
#include <thread>

using oo::Renderer;

std::vector<Renderer::Tile>
Renderer::generateTiles(int xTileSize, int yTileSize, int numSamples,
                        int samplesPerTile, int seed) const {
  return Renderer::generateTiles(renderParams_.width, renderParams_.height,
                                 xTileSize, yTileSize, numSamples,
                                 samplesPerTile, seed);
}

std::vector<Renderer::Tile>
Renderer::generateTiles(int width, int height, int xTileSize, int yTileSize,
                        int numSamples, int samplesPerTile, int seed) {
  std::mt19937 rng(seed);
  std::vector<Tile> tiles;
  for (int y = 0; y < height; y += yTileSize) {
    int yBegin = y;
    int yEnd = std::min(y + yTileSize, height);
    for (int x = 0; x < width; x += xTileSize) {
      int xBegin = x;
      int xEnd = std::min(x + xTileSize, width);
      int midX = (xEnd + xBegin) / 2;
      int midY = (yEnd + yBegin) / 2;
      int centreX = width / 2;
      int centreY = height / 2;
      size_t distanceSqr = (midX - centreX) * (midX - centreX)
                           + (midY - centreY) * (midY - centreY);
      for (int s = 0; s < numSamples; s += samplesPerTile) {
        int nSamples = std::min(s + samplesPerTile, numSamples) - s;
        tiles.emplace_back(
            Tile{xBegin, xEnd, yBegin, yEnd, nSamples, s, distanceSqr, rng()});
      }
    }
  }
  std::sort(tiles.begin(), tiles.end(), [](const Tile &lhs, const Tile &rhs) {
    return lhs.key() > rhs.key();
  });
  return tiles;
}

class Renderer::Sampler : public oo::Material::RadianceSampler {
  const Renderer &renderer_;
  std::mt19937 &rng_;
  int depth_;

public:
  Sampler(const Renderer &renderer, std::mt19937 &rng, int depth)
      : renderer_(renderer), rng_(rng), depth_(depth) {}
  [[nodiscard]] Vec3 sample(const Ray &ray) const override {
    return renderer_.radiance(rng_, ray, depth_);
  }
};

Vec3 Renderer::radiance(std::mt19937 &rng, const Ray &ray, int depth) const {
  if (depth >= renderParams_.maxDepth)
    return Vec3();
  int numUSamples = depth == 0 ? renderParams_.firstBounceUSamples : 1;
  int numVSamples = depth == 0 ? renderParams_.firstBounceVSamples : 1;
  Primitive::IntersectionRecord intersectionRecord;
  if (!scene_.intersect(ray, intersectionRecord))
    return scene_.environment(ray);

  const auto &material = *intersectionRecord.material;
  if (renderParams_.preview)
    return material.previewColour();
  const auto &hit = intersectionRecord.hit;

  Vec3 result;
  Sampler sampler(*this, rng, depth + 1);

  // Sample evenly with random offset.
  std::uniform_real_distribution<> unit(0, 1.0);
  for (auto uSample = 0; uSample < numUSamples; ++uSample) {
    for (auto vSample = 0; vSample < numVSamples; ++vSample) {
      auto u = (uSample + unit(rng)) / numUSamples;
      auto v = (vSample + unit(rng)) / numVSamples;
      auto p = unit(rng);

      // TODO point out this is how we deal with "recursion" or encapsulation
      // between material and renderer.
      result += material.sample(hit, ray, sampler, u, v, p);
    }
  }
  return material.totalEmission(result / (numUSamples * numVSamples));
}

ArrayOutput Renderer::render(
    const std::function<void(const ArrayOutput &)> &updateFunc) const {
  int curSample = 0;
  auto launch = [&] {
    return std::async(std::launch::async, [&] {
      ArrayOutput output(renderParams_.width, renderParams_.height);
      std::mt19937 rng(renderParams_.seed + curSample++);
      for (auto y = 0; y < renderParams_.height; ++y) {
        for (auto x = 0; x < renderParams_.width; ++x) {
          auto ray = camera_.randomRay(x, y, rng);
          output.addSamples(x, y, radiance(rng, ray, 0), 1);
        }
      }
      return output;
    });
  };

  std::vector<std::future<ArrayOutput>> futures;
  auto ensureMaxCpus = [&] {
    auto numLeft = renderParams_.samplesPerPixel - curSample;
    auto numSpareCpus = renderParams_.maxCpus - futures.size();
    auto numToSpawn = std::min<size_t>(numLeft, numSpareCpus);
    for (size_t i = 0u; i < numToSpawn; ++i)
      futures.emplace_back(launch());
  };

  size_t numDone = 0;
  ArrayOutput output(renderParams_.width, renderParams_.height);
  Progressifier progressifier(renderParams_.samplesPerPixel);
  do {
    ensureMaxCpus();
    const auto firstDone =
        std::find_if(futures.begin(), futures.end(), [&](auto &f) {
          return f.wait_for(std::chrono::milliseconds(1))
                 == std::future_status ::ready;
        });
    if (firstDone != futures.end()) {
      output += firstDone->get();
      numDone++;
      progressifier.update(numDone);
      updateFunc(output);
      futures.erase(firstDone);
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

  } while (curSample < renderParams_.samplesPerPixel);
  return output;
}

ArrayOutput Renderer::renderTiled(
    std::function<void(const ArrayOutput &)> updateFunc) const {
  ArrayOutput output(renderParams_.width, renderParams_.height);

  auto renderPixel = [this](std::mt19937 &rng, int pixelX, int pixelY,
                            int numSamples) {
    Vec3 colour;
    for (int sample = 0; sample < numSamples; ++sample) {
      auto ray = camera_.randomRay(pixelX, pixelY, rng);
      colour += radiance(rng, ray, 0);
    }
    return colour;
  };

  WorkQueue<Tile> queue(generateTiles(16, 16, renderParams_.samplesPerPixel, 8,
                                      renderParams_.seed));

  auto worker = [&] {
    for (;;) {
      auto tileOpt = queue.pop([&] { updateFunc(output); });
      if (!tileOpt)
        break;
      auto &tile = *tileOpt;

      std::mt19937 rng(tile.randomPrio);
      for (int y = tile.yBegin; y < tile.yEnd; ++y) {
        for (int x = tile.xBegin; x < tile.xEnd; ++x) {
          output.addSamples(x, y, renderPixel(rng, x, y, tile.samples),
                            tile.samples);
        }
      }
    }
  };
  std::vector<std::thread> threads{static_cast<size_t>(renderParams_.maxCpus)};
  std::generate(threads.begin(), threads.end(),
                [&]() { return std::thread(worker); });
  for (auto &t : threads)
    t.join();

  return output;
}
