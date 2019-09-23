#include "Render.h"

#include "Primitive.h"
#include "Scene.h"
#include "math/Camera.h"
#include "math/Samples.h"
#include "optional.hpp"
#include "util/ArrayOutput.h"
#include "util/Progressifier.h"

#include <future>
#include <range/v3/all.hpp>

namespace fp {

struct IntersectionRecord {
  Hit hit;
  const MaterialSpec &material;
};

struct IntersectVisitor {
  const Ray &ray;

  template <typename Primitive>
  auto operator()(const Primitive &primitive) const {
    return primitive.shape.intersect(ray).map([&primitive](auto hit) {
      return IntersectionRecord{hit, primitive.material};
    });
  }
};

tl::optional<IntersectionRecord> intersect(const Primitive &primitive,
                                           const Ray &ray) {
  return std::visit(IntersectVisitor{ray}, primitive);
}

tl::optional<IntersectionRecord> intersect(const Scene &scene, const Ray &ray) {
  tl::optional<IntersectionRecord> nearest;
  for (auto &primitive : scene.primitives) {
    auto thisIntersection = intersect(primitive, ray);
    if (thisIntersection
        && (!nearest || thisIntersection->hit.distance < nearest->hit.distance))
      nearest.emplace(*thisIntersection);
  }
  return nearest;
}

template <typename RadianceFunc>
Vec3 radianceAtIntersection(RadianceFunc &&radiance,
                            const IntersectionRecord &intersectionRecord,
                            const Ray &ray, const OrthoNormalBasis &basis,
                            double u, double v, double p) {
  std::uniform_real_distribution<> unit(0, 1.0);
  const auto &mat = intersectionRecord.material;
  const auto &hit = intersectionRecord.hit;

  const auto [iorFrom, iorTo] =
      hit.inside ? std::make_pair(mat.indexOfRefraction, 1.0)
                 : std::make_pair(1.0, mat.indexOfRefraction);
  const auto reflectivity =
      mat.reflectivity < 0
          ? hit.normal.reflectance(ray.direction(), iorFrom, iorTo)
          : mat.reflectivity;

  if (p < reflectivity) {
    const auto newRay =
        Ray(hit.position, coneSample(hit.normal.reflect(ray.direction()),
                                     mat.reflectionConeAngleRadians, u, v));
    return radiance(newRay);
  } else {
    const auto newRay = Ray(hit.position, hemisphereSample(basis, u, v));
    return mat.diffuse * radiance(newRay);
  }
}

Vec3 radiance(const Scene &scene, std::mt19937 &rng, const Ray &ray, int depth,
              const RenderParams &renderParams) {
  using namespace ranges;
  const auto numUSamples = depth == 0 ? renderParams.firstBounceUSamples : 1;
  const auto numVSamples = depth == 0 ? renderParams.firstBounceVSamples : 1;
  if (depth >= renderParams.maxDepth)
    return Vec3();
  const auto intersectionRecord = intersect(scene, ray);
  if (!intersectionRecord)
    return scene.environment;

  const auto &mat = intersectionRecord->material;
  const auto &hit = intersectionRecord->hit;
  if (renderParams.preview)
    return mat.diffuse;

  // Create a coordinate system local to the point, where the z is the
  // normal at this point.
  const auto basis = OrthoNormalBasis::fromZ(hit.normal);
  std::uniform_real_distribution<> unit;

  auto toUVSample = [&rng, &unit, numUSamples, numVSamples](auto vu) {
    auto [v, u] = vu;
    const auto sampleU = (u + unit(rng)) / numUSamples;
    const auto sampleV = (v + unit(rng)) / numVSamples;
    return std::make_pair(sampleU, sampleV);
  };

  auto radianceForRay = [&](const Ray &ray) {
    return radiance(scene, rng, ray, depth + 1, renderParams);
  };

  const auto incomingLight = accumulate(
      views::cartesian_product(views::ints(0, numVSamples),
                               views::ints(0, numUSamples))
          | views::transform(toUVSample) | views::transform([&](auto s) {
              return radianceAtIntersection(radianceForRay, *intersectionRecord,
                                            ray, basis, s.first, s.second,
                                            unit(rng));
            }),
      Vec3());
  return mat.emission + incomingLight / (numUSamples * numVSamples);
}

ArrayOutput renderWholeScreen(const Camera &camera, const Scene &scene,
                              size_t seed, const RenderParams &renderParams) {
  using namespace ranges;
  auto renderOnePixel = [seed, &renderParams, &camera, &scene](auto tuple) {
    auto [y, x] = tuple;
    std::mt19937 rng(renderParams.height * renderParams.width * seed
                     + x * renderParams.width + y);
    return radiance(scene, rng, camera.randomRay(x, y, rng), 0, renderParams);
  };
  auto renderedPixelsView =
      views::cartesian_product(views::ints(0, renderParams.height),
                               views::ints(0, renderParams.width))
      | views::transform(renderOnePixel);
  return ArrayOutput(renderParams.width, renderParams.height,
                     renderedPixelsView);
}

ArrayOutput render(const Camera &camera, const Scene &scene,
                   const RenderParams &renderParams,
                   const std::function<void(const ArrayOutput &)> &updateFunc) {
  // TODO no raw loops...maybe return whole "Samples" of an entire screen and
  // accumulate separately?
  // future from an async()
  auto seed = renderParams.seed;
  size_t numDone = 0;
  ArrayOutput output(renderParams.width, renderParams.height);
  Progressifier progressifier(renderParams.samplesPerPixel);
  for (int sample = 0; sample < renderParams.samplesPerPixel;
       sample += renderParams.maxCpus) {
    std::vector<std::future<ArrayOutput>> futures;
    for (int ss = sample; ss < std::min(renderParams.samplesPerPixel,
                                        sample + renderParams.maxCpus);
         ++ss) {
      futures.emplace_back(std::async(std::launch::async, [&] {
        return renderWholeScreen(camera, scene, seed++, renderParams);
      }));
    }
    for (auto &&future : futures) {
      future.wait();
      output += future.get();
      numDone++;
      progressifier.update(numDone);
      updateFunc(output);
    }
  }
  progressifier.update(numDone);
  return output;
}

}
