#include "Scene.h"
#include "math/Epsilon.h"
#include "math/OrthoNormalBasis.h"
#include "math/Samples.h"
#include "util/Progressifier.h"
#include "util/Unpredictable.h"

#include <future>

using dod::IntersectionRecord;
using dod::Scene;

std::optional<IntersectionRecord>
Scene::intersectSpheres(const Ray &ray, double nearerThan) const {
  double currentNearestDist = nearerThan;
  std::optional<size_t> nearestIndex;
  for (size_t sphereIndex = 0; sphereIndex < spheres_.size(); ++sphereIndex) {
    // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
    auto op = spheres_[sphereIndex].centre - ray.origin();
    auto b = op.dot(ray.direction().toVec3());
    auto determinant =
        b * b - op.lengthSquared() + spheres_[sphereIndex].radiusSquared;
    if (determinant < 0)
      continue;

    determinant = sqrt(determinant);
    auto minusT = b - determinant;
    auto plusT = b + determinant;
    if (minusT < Epsilon && plusT < Epsilon)
      continue;

    auto t = minusT > Epsilon ? minusT : plusT;
    if (t < currentNearestDist) {
      nearestIndex = sphereIndex;
      currentNearestDist = t;
    }
  }
  if (!nearestIndex)
    return {};

  auto hitPosition = ray.positionAlong(currentNearestDist);
  auto normal = (hitPosition - spheres_[*nearestIndex].centre).normalised();
  bool inside = normal.dot(ray.direction()) > 0;
  if (inside)
    normal = -normal;
  return IntersectionRecord{
      Hit{currentNearestDist, inside, hitPosition, normal},
      sphereMaterials_[*nearestIndex]};
}

std::optional<IntersectionRecord>
Scene::intersectTriangles(const Ray &ray, double nearerThan) const {
  double currentNearestDist = nearerThan;
  struct Nearest {
    size_t index;
    double det;
    double u;
    double v;
  };

  std::optional<Nearest> nearest;
  for (size_t i = 0; i < triangleVerts_.size(); ++i) {
    const auto &tv = triangleVerts_[i];
    const auto pVec = ray.direction().cross(tv.vVector());
    const auto det = tv.uVector().dot(pVec);
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < Epsilon)
      continue;

    const auto invDet = 1.0 / det;
    const auto tVec = ray.origin() - tv.vertex(0);
    const auto u = tVec.dot(pVec) * invDet;
    const auto qVec = tVec.cross(tv.uVector());
    const auto v = ray.direction().dot(qVec) * invDet;

    // This is an important optimisation:
    // The overall chance of the ray intersecting this triangle is likely to be
    // extremely small: so this comparison is almost always "true". HOWEVER, if
    // the compiler implements this as a sequence of compare-and-branches, one
    // for each continue, then each individual branch is essentially random and
    // unpredictable. If the compiler is forced to emit only one branch (on the
    // total result of the expression), then the branch is predictable. This
    // makes a huge performance difference!
    // I found GCC (9.1) very sensitive to the ordering of comparisons if I used
    // regular logical or here: this bitwise or pretty much guarantees a single
    // branch.
    // (extra parens around variables are to prevent clang-format from getting
    // confused).
    if (Unpredictable::any((u) < 0.0, u > 1.0, (v) < 0.0, u + v > 1))
      continue;

    const auto t = tv.vVector().dot(qVec) * invDet;

    if (t > Epsilon && t < currentNearestDist) {
      nearest = Nearest{i, det, u, v};
      currentNearestDist = t;
    }
  }
  if (!nearest)
    return {};
  auto &tn = triangleNormals_[nearest->index];
  auto normalUdelta = tn[1].toVec3() - tn[0].toVec3();
  auto normalVdelta = tn[2].toVec3() - tn[0].toVec3();
  // TODO: proper barycentric coordinates
  const auto normal = ((nearest->u * normalUdelta) + (nearest->v * normalVdelta)
                       + tn[0].toVec3())
                          .normalised();
  bool backfacing = nearest->det < Epsilon;
  return IntersectionRecord{Hit{currentNearestDist, backfacing,
                                ray.positionAlong(currentNearestDist),
                                backfacing ? -normal : normal},
                            triangleMaterials_[nearest->index]};
}

std::optional<IntersectionRecord> Scene::intersect(const Ray &ray) const {
  auto sphereRec =
      intersectSpheres(ray, std::numeric_limits<double>::infinity());
  auto triangleRec = intersectTriangles(
      ray, sphereRec ? sphereRec->hit.distance
                     : std::numeric_limits<double>::infinity());
  return triangleRec ? triangleRec : sphereRec;
}

Vec3 Scene::radiance(std::mt19937 &rng, const Ray &ray, int depth,
                     const RenderParams &renderParams) const {
  int numUSamples = depth == 0 ? renderParams.firstBounceUSamples : 1;
  int numVSamples = depth == 0 ? renderParams.firstBounceVSamples : 1;
  if (depth >= renderParams.maxDepth)
    return Vec3();

  const auto intersectionRecord = intersect(ray);
  if (!intersectionRecord)
    return environment_;

  const auto &mat = intersectionRecord->material;
  const auto &hit = intersectionRecord->hit;
  if (renderParams.preview)
    return mat.diffuse;

  const auto [iorFrom, iorTo] =
      hit.inside ? std::make_pair(mat.indexOfRefraction, 1.0)
                 : std::make_pair(1.0, mat.indexOfRefraction);
  const auto reflectivity =
      mat.reflectivity < 0
          ? hit.normal.reflectance(ray.direction(), iorFrom, iorTo)
          : mat.reflectivity;

  // Sample evenly with random offset.
  std::uniform_real_distribution<> unit(0, 1.0);
  // Create a coordinate system local to the point, where the z is the
  // normal at this point.
  const auto basis = OrthoNormalBasis::fromZ(hit.normal);
  Vec3 result;

  for (auto uSample = 0; uSample < numUSamples; ++uSample) {
    for (auto vSample = 0; vSample < numVSamples; ++vSample) {
      const auto u = (static_cast<double>(uSample) + unit(rng))
                     / static_cast<double>(numUSamples);
      const auto v = (static_cast<double>(vSample) + unit(rng))
                     / static_cast<double>(numVSamples);
      const auto p = unit(rng);

      if (p < reflectivity) {
        auto newRay =
            Ray(hit.position, coneSample(hit.normal.reflect(ray.direction()),
                                         mat.reflectionConeAngleRadians, u, v));

        result += mat.emission + radiance(rng, newRay, depth + 1, renderParams);
      } else {
        auto newRay = Ray(hit.position, hemisphereSample(basis, u, v));

        result +=
            mat.emission
            + mat.diffuse * radiance(rng, newRay, depth + 1, renderParams);
      }
    }
  }
  return result / (numUSamples * numVSamples);
}

void Scene::addTriangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2,
                        const MaterialSpec &material) {
  auto &tv = triangleVerts_.emplace_back(TriangleVertices{v0, v1, v2});
  triangleNormals_.emplace_back(
      TriangleNormals{tv.faceNormal(), tv.faceNormal(), tv.faceNormal()});
  triangleMaterials_.emplace_back(material);
}

void Scene::addSphere(const Vec3 &centre, double radius,
                      const MaterialSpec &material) {
  spheres_.emplace_back(centre, radius);
  sphereMaterials_.emplace_back(material);
}

void Scene::setEnvironmentColour(const Vec3 &colour) { environment_ = colour; }

ArrayOutput
Scene::render(const Camera &camera, const RenderParams &renderParams,
              const std::function<void(ArrayOutput &output)> &updateFunc) {
  auto width = renderParams.width;
  auto height = renderParams.height;

  // TODO no raw loops...maybe return whole "Samples" of an entire screen and
  // accumulate separately? then feeds into a nice multithreaded future based
  // thing?

  int curSample = 0;
  auto launch = [&] {
    return std::async(std::launch::async, [&] {
      ArrayOutput output(width, height);
      std::mt19937 rng(renderParams.seed + curSample++);
      for (auto y = 0; y < height; ++y) {
        for (auto x = 0; x < width; ++x) {
          auto ray = camera.randomRay(x, y, rng);
          output.addSamples(x, y, radiance(rng, ray, 0, renderParams), 1);
        }
      }
      return output;
    });
  };

  std::vector<std::future<ArrayOutput>> futures;
  auto ensureMaxCpus = [&] {
    auto numLeft = renderParams.samplesPerPixel - curSample;
    auto numSpareCpus = renderParams.maxCpus - futures.size();
    auto numToSpawn = std::min<size_t>(numLeft, numSpareCpus);
    for (auto i = 0u; i < numToSpawn; ++i)
      futures.emplace_back(launch());
  };

  size_t numDone = 0;
  ArrayOutput output(width, height);
  Progressifier progressifier(renderParams.samplesPerPixel);
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

  } while (curSample < renderParams.samplesPerPixel);

  return output;
}
