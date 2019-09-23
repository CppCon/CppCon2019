#include "PngWriter.h"

#include "dod/Scene.h"
#include "fp/Render.h"
#include "fp/SceneBuilder.h"
#include "math/Camera.h"
#include "math/Vec3.h"
#include "oo/Renderer.h"
#include "oo/SceneBuilder.h"
#include "util/ArrayOutput.h"
#include "util/ObjLoader.h"
#include "util/RenderParams.h"

#include <clara.hpp>
#include <date/chrono_io.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

namespace {

struct DirRelativeOpener : ObjLoaderOpener {
  std::string dir_;
  explicit DirRelativeOpener(std::string dir) : dir_(std::move(dir)) {}
  [[nodiscard]] std::unique_ptr<std::istream>
  open(const std::string &filename) override {
    auto fullname = dir_ + "/" + filename;
    auto res = std::make_unique<std::ifstream>(fullname);
    if (!*res)
      throw std::runtime_error("Unable to open " + fullname);
    return res;
  }
};

Vec3 hexColour(uint32_t hex) {
  auto c = [](unsigned x) { return pow((x & 0xffu) / 255.0, 2.2); };
  return Vec3(c(hex >> 16u), c(hex >> 8u), c(hex));
}

template <typename SB>
void addCube(SB &sb, const Vec3 &low, const Vec3 &high,
             const MaterialSpec &material) {
  auto T = [&](unsigned bit) {
    bool x = bit & 4u;
    bool y = bit & 2u;
    bool z = bit & 1u;
    return Vec3(x ? low.x() : high.x(), y ? low.y() : high.y(),
                z ? low.z() : high.z());
  };
  sb.addTriangle(T(0b000), T(0b100), T(0b110), material);
  sb.addTriangle(T(0b000), T(0b110), T(0b010), material);
  sb.addTriangle(T(0b001), T(0b101), T(0b111), material);
  sb.addTriangle(T(0b001), T(0b111), T(0b011), material);
  sb.addTriangle(T(0b000), T(0b100), T(0b101), material);
  sb.addTriangle(T(0b000), T(0b101), T(0b001), material);
  sb.addTriangle(T(0b010), T(0b110), T(0b111), material);
  sb.addTriangle(T(0b010), T(0b111), T(0b011), material);
  sb.addTriangle(T(0b000), T(0b010), T(0b011), material);
  sb.addTriangle(T(0b000), T(0b011), T(0b001), material);
  sb.addTriangle(T(0b100), T(0b110), T(0b111), material);
  sb.addTriangle(T(0b100), T(0b111), T(0b101), material);
}

template <typename SB>
Camera createCornellScene(SB &sb, const RenderParams &renderParams) {
  DirRelativeOpener opener("scenes");
  auto in = opener.open("CornellBox-Original.obj");
  loadObjFile(*in, opener, sb);
  sb.addSphere(
      Vec3(-0.38, 0.281, 0.38), 0.28,
      MaterialSpec::makeReflective(Vec3(0.999, 0.999, 0.999), 0.95, 5));
  sb.setEnvironmentColour(Vec3(0.725, 0.71, 0.68) * 0.1);
  Vec3 camPos(0, 1, 3);
  Vec3 camUp(0, 1, 0);
  Vec3 camLookAt(0, 1, 0);
  double verticalFov = 50.0;
  Camera camera(camPos, camLookAt, camUp.normalised(), renderParams.width,
                renderParams.height, verticalFov);
  camera.setFocus(Vec3(0, 0, 0), 0.01);
  return camera;
}

template <typename SB>
auto createSuzanneScene(SB &sb, const RenderParams &renderParams) {
  DirRelativeOpener opener("scenes");
  auto in = opener.open("suzanne.obj");
  loadObjFile(*in, opener, sb);

  auto lightMat = MaterialSpec::makeLight(Vec3(4, 4, 4));
  sb.addSphere(Vec3(0.5, 1, 3), 1, lightMat);
  sb.addSphere(Vec3(1, 1, 3), 1, lightMat);

  auto boxMat = MaterialSpec::makeDiffuse(Vec3(0.20, 0.30, 0.36));
  auto tl = Vec3(-5, -5, -1);
  auto tr = Vec3(5, -5, -1);
  auto bl = Vec3(-5, 5, -1);
  auto br = Vec3(5, 5, -1);
  sb.addTriangle(tl, tr, bl, boxMat);
  sb.addTriangle(tr, bl, br, boxMat);

  Vec3 camPos(1, -0.45, 4);
  Vec3 camLookAt(1, -0.6, 0.4);
  Vec3 camUp(0, 1, 0);
  double verticalFov = 40.0;
  Camera camera(camPos, camLookAt, camUp.normalised(), renderParams.width,
                renderParams.height, verticalFov);
  camera.setFocus(camLookAt, 0.01);
  return camera;
}

template <typename SB>
auto createCeScene(SB &sb, const RenderParams &renderParams) {
  DirRelativeOpener opener("scenes");
  auto in = opener.open("ce.obj");
  loadObjFile(*in, opener, sb);

  auto brightLight = MaterialSpec::makeLight(Vec3(1, 1, 1) * 10);
  sb.addSphere(Vec3(0, 1.6, 0), 1.0, brightLight);
  auto dullLight = MaterialSpec::makeLight(Vec3(2.27, 3, 2.97) * 0.25);
  sb.addSphere(Vec3(-0.2, 5.9, -0.3), 5.0, dullLight);

  sb.addSphere(Vec3(), 10, MaterialSpec::makeDiffuse(Vec3(0.2, 0.2, 0.2)));

  Vec3 camPos(0.27, 1.15, 0.36);
  Vec3 camLookAt(0, 0, 0);
  Vec3 camUp(0, 0, -1);
  double verticalFov = 40.0;
  Camera camera(camPos, camLookAt, camUp.normalised(), renderParams.width,
                renderParams.height, verticalFov);
  camera.setFocus(camLookAt, 0.01);
  return camera;
}

template <typename SB>
auto createSingleSphereScene(SB &sb, const RenderParams &renderParams) {
  Vec3 camPos(0, 0, -3.2);
  Vec3 camLookAt(0, 0, 0);
  Vec3 camUp(0, 1, 0);
  double verticalFov = 40.0;
  Camera camera(camPos, camLookAt, camUp.normalised(), renderParams.width,
                renderParams.height, verticalFov);

  auto lightRadius = 3.0;
  auto lightOffset = Vec3(6, 6, 0);
  auto lightMat = MaterialSpec::makeLight(Vec3(1, 1, 1) * 8);
  sb.addSphere(camPos + lightOffset - Vec3(0, 0, lightRadius), lightRadius,
               lightMat);

  auto sphereMat = MaterialSpec::makeDiffuse(Vec3(0.2, 0.2, 0.2));
  sphereMat.indexOfRefraction = 1.3;
  sphereMat.reflectionConeAngleRadians = 0.05;
  sb.addSphere(Vec3(), 1, sphereMat);

  auto worldMat = MaterialSpec::makeDiffuse(Vec3(0.2, 0.2, 0.5));
  sb.addSphere(Vec3(), 10, worldMat);

  return camera;
}

template <typename SB>
auto createMultiSphereScene(SB &sb, const RenderParams &renderParams) {
  Vec3 camPos(0, 0, -3.2);
  Vec3 camLookAt(0, 0, 0);
  Vec3 camUp(0, 1, 0);
  double verticalFov = 40.0;
  Camera camera(camPos, camLookAt, camUp.normalised(), renderParams.width,
                renderParams.height, verticalFov);

  auto lightRadius = 3.0;
  auto lightOffset = Vec3(6, 6, 0);
  auto lightMat = MaterialSpec::makeLight(Vec3(1, 1, 1) * 8);
  sb.addSphere(camPos + lightOffset - Vec3(0, 0, lightRadius), lightRadius,
               lightMat);

  const auto sphereRadius = 1.0 / 5.0;
  const auto sphereGap = sphereRadius * 2.15;
  for (int y = -2; y <= 2; ++y) {
    for (int x = -4; x <= 4; ++x) {
      auto sphereMat = MaterialSpec::makeDiffuse(Vec3(0.90, 0.91, 0.92));
      sphereMat.reflectionConeAngleRadians = 0.075 * (x + 4);
      sphereMat.indexOfRefraction = 1.0 + 0.15 * (y + 2);
      sb.addSphere(Vec3(x * sphereGap, y * sphereGap, 0), sphereRadius,
                   sphereMat);
    }
  }

  auto worldMat = MaterialSpec::makeDiffuse(Vec3(0.2, 0.2, 0.5));
  sb.addSphere(Vec3(), 10, worldMat);

  return camera;
}

template <typename SB>
auto createExample1Scene(SB &sb, const RenderParams &renderParams) {
  // From @fogleman's pt example1.go
  sb.addSphere(Vec3(1.5, 1.25, 0), 1.25,
               MaterialSpec::makeSpecular(hexColour(0x004358), 1.3));
  sb.addSphere(Vec3(-1, 1, 2), 1.0,
               MaterialSpec::makeSpecular(hexColour(0xffe11a), 1.3));
  sb.addSphere(Vec3(-2.5, 0.75, 0), 0.75,
               MaterialSpec::makeSpecular(hexColour(0xfd7400), 1.3));
  // TODO: clear materials...
  sb.addSphere(Vec3(-0.75, 0.5, -1), 0.5,
               MaterialSpec::makeSpecular(hexColour(0), 1.3));
  addCube(sb, Vec3(-10, -1, -10), Vec3(10, 0, 10),
          MaterialSpec::makeGlossy(Vec3(1, 1, 1), 1.1, 10.0));

  sb.addSphere(Vec3(-1.5, 4, 0), 0.5,
               MaterialSpec::makeLight(Vec3(1, 1, 1) * 30));

  Vec3 camPos(0, 2, -5);
  Vec3 camLookAt(0, 0.25, 3);
  Vec3 camUp(0, 1, 0);
  double verticalFov = 45.0;
  Camera camera(camPos, camLookAt, camUp.normalised(), renderParams.width,
                renderParams.height, verticalFov);
  camera.setFocus(Vec3(-0.75, 1, -1), 0.1);

  return camera;
}

template <typename SB>
auto createBbcOwlScene(SB &sb, const RenderParams &renderParams) {
  using namespace std::literals;
  constexpr auto owlHeight = 21;
  constexpr std::string_view owl[owlHeight] = {
      // clang-format off
      "* * * * * * * * *"sv,
      " *     * *     * "sv,
      "*   *   *   *   *"sv,
      "   * *     * *   "sv,
      "*   *       *   *"sv,
      " *     * *     * "sv,
      "* *     *     * *"sv,
      " * *         *   "sv,
      "* * * * * * *   *"sv,
      " * * * *         "sv,
      "* * * * *       *"sv,
      " * * * *         "sv,
      "  * * * *       *"sv,
      "   * * * *       "sv,
      "    * * * *     *"sv,
      "     * * * *     "sv,
      "      * * * *   *"sv,
      "       * * * *   "sv,
      "      *   *   * *"sv,
      " * * * * * *   * "sv,
      "                *"sv
      // clang-format on
  };
  constexpr auto owlWidth = owl[0].length();
  const auto sphereSpacing = 0.1;
  const auto sphereSize = sphereSpacing * 0.7;
  auto y = owlHeight * sphereSpacing - sphereSpacing / 2;
  for (auto &&line : owl) {
    auto x = owlWidth * sphereSpacing / 2;
    for (auto c : line) {
      if (c == '*') {
        sb.addSphere(Vec3(x, y, 0), sphereSize,
                     MaterialSpec::makeSpecular(hexColour(0xfeffd5), 1.3));
      }
      x -= sphereSpacing;
    }
    y -= sphereSpacing;
  }
  auto planeMat = MaterialSpec::makeReflective(Vec3(0.2, 0.2, 0.2), 0.75, 3.0);
  planeMat.indexOfRefraction = 1.5;
  addCube(sb, Vec3(-10, -1, -10), Vec3(10, 0, 10), planeMat);

  sb.addSphere(Vec3(-1.5, 4.0, -1), 0.75,
               MaterialSpec::makeLight(Vec3(1, 1, 1) * 30));

  sb.setEnvironmentColour(Vec3(0.2, 0.2, 0.5) * 0.05);

  Vec3 camPos(4, 2.0, -5);
  Vec3 camLookAt(0, 0.5, 0);
  Vec3 camUp(0, 1, 0);
  double verticalFov = 33.0;
  Camera camera(camPos, camLookAt, camUp.normalised(), renderParams.width,
                renderParams.height, verticalFov);
  camera.setFocus(Vec3(0, 0.5, 0), 0.1);

  return camera;
}

template <typename SB>
auto createScene(SB &sb, const std::string &sceneName,
                 const RenderParams &renderParams) {
  if (sceneName == "cornell")
    return createCornellScene(sb, renderParams);
  if (sceneName == "suzanne")
    return createSuzanneScene(sb, renderParams);
  if (sceneName == "ce")
    return createCeScene(sb, renderParams);
  if (sceneName == "single-sphere")
    return createSingleSphereScene(sb, renderParams);
  if (sceneName == "multi-sphere")
    return createMultiSphereScene(sb, renderParams);
  if (sceneName == "example1")
    return createExample1Scene(sb, renderParams);
  if (sceneName == "bbc-owl")
    return createBbcOwlScene(sb, renderParams);
  throw std::runtime_error("Unknown scene " + sceneName);
}

struct StatsSceneBuilder {
  int numTriangles{};
  int numSpheres{};

  void addTriangle(...) { numTriangles++; }

  void addSphere(...) { numSpheres++; }
  void setEnvironmentColour(...) {}

  void report() {
    std::cout << "Scene contains " << numTriangles << " triangles and "
              << numSpheres << " spheres.\n";
  }
};

ArrayOutput doRender(const std::string &way, const std::string &sceneName,
                     const RenderParams &renderParams,
                     std::chrono::seconds saveEvery,
                     std::function<void(const ArrayOutput &)> save) {
  using namespace std::chrono_literals;
  auto nextSave = std::chrono::system_clock::now() + saveEvery;
  auto throttledSave = [&](const ArrayOutput &output) {
    if (saveEvery == 0s)
      return;
    // TODO: save is not thread safe even slightly, and yet it still blocks
    // the threads. this is terrible. Should have a thread safe result queue
    // and a single thread reading from it.
    auto now = std::chrono::system_clock::now();
    if (now > nextSave) {
      save(output);
      nextSave = now + saveEvery;
    }
  };

  StatsSceneBuilder ssb;
  createScene(ssb, sceneName, renderParams);
  ssb.report();

  std::optional<ArrayOutput> result;
  if (way == "oo") {
    oo::SceneBuilder sceneBuilder;
    auto camera = createScene(sceneBuilder, sceneName, renderParams);
    oo::Renderer renderer(sceneBuilder.scene(), camera, renderParams);
    return renderer.render(throttledSave);
  } else if (way == "fp") {
    fp::SceneBuilder sceneBuilder;
    auto camera = createScene(sceneBuilder, sceneName, renderParams);
    return fp::render(camera, sceneBuilder.scene(), renderParams,
                      throttledSave);
  } else if (way == "dod") {
    dod::Scene scene;
    auto camera = createScene(scene, sceneName, renderParams);
    return scene.render(camera, renderParams, throttledSave);
  } else {
    throw std::runtime_error("Unknown way " + way + "\n");
  }
}
}

int main(int argc, const char *argv[]) {
  using namespace clara;
  using namespace std::literals;

  bool help = false;
  bool raw = false;
  int saveEvery = 30;
  RenderParams renderParams;
  std::string way = "oo";
  std::string sceneName = "cornell";
  std::string outputName;

  auto cli =
      Opt(renderParams.width, "width")["-w"]["--width"]("output image width")
      | Opt(renderParams.height,
            "height")["-h"]["--height"]("output image height")
      | Opt(renderParams.maxCpus,
            "#cpus")["--max-cpus"]("maximum number of CPUs to use (0 for all)")
      | Opt(renderParams.samplesPerPixel,
            "samples")["--spp"]("number of samples per pixel")
      | Opt(renderParams.firstBounceUSamples,
            "samples")["--first-bounce-u"]("number of first bounce u samples")
      | Opt(renderParams.firstBounceVSamples,
            "samples")["--first-bounce-v"]("number of first bounce v samples")
      | Opt(renderParams.maxDepth,
            "depth")["--max-depth"]("maximum recursion depth")
      | Opt(renderParams.seed,
            "seed")["--seed"]("set rendering seed (0 to use random seed)")
      | Opt(renderParams.preview)["--preview"]("super quick preview")
      | Opt(saveEvery, "secs")["--save-every"](
          "periodically save (every secs), 0 to disable")
      | Opt(way, "way")["--way"]("which way, oo (the default), fp or dod")
      | Opt(sceneName, "scene")["--scene"]("which scene to render")
      | Opt(raw)["--raw"]("output in raw form")
      | Arg(outputName, "output")("output filename").required() | Help(help);

  auto result = cli.parse(Args(argc, argv));
  if (!result) {
    std::cerr << "Error in command line: " << result.errorMessage() << '\n';
    exit(1);
  }
  if (help) {
    std::cout << cli;
    exit(0);
  }

  if (outputName.empty()) { // https://github.com/catchorg/Clara/issues/39
    std::cerr << "Missing output filename.\n" << cli;
    exit(1);
  }

  if (renderParams.maxCpus == 0) {
    renderParams.maxCpus =
        static_cast<int>(std::thread::hardware_concurrency());
  }

  if (renderParams.seed == 0) {
    std::random_device device;
    renderParams.seed = device();
  }

  std::function<void(const ArrayOutput &)> save;

  if (raw) {
    save = [outputName](const ArrayOutput &output) { output.save(outputName); };
  } else {
    save = [outputName](const ArrayOutput &output) {
      PngWriter pw(outputName.c_str(), output.width(), output.height());
      if (!pw.ok()) {
        std::cerr << "Unable to save PNG\n";
        return;
      }

      for (int y = 0; y < output.height(); ++y) {
        std::uint8_t row[output.width() * 3];
        for (int x = 0; x < output.width(); ++x) {
          auto colour = output.pixelAt(x, y);
          for (int component = 0; component < 3; ++component)
            row[x * 3 + component] = colour[component];
        }
        pw.addRow(row);
      }
    };
  }

  auto startTime = std::chrono::system_clock::now();
  auto output = doRender(way, sceneName, renderParams,
                         std::chrono::seconds(saveEvery), save);
  auto endTime = std::chrono::system_clock::now();

  save(output);

  using namespace date;
  auto timeTaken = endTime - startTime;
  auto totalSamples = output.totalSamples();
  std::cout << "Took "
            << std::chrono::duration_cast<std::chrono::seconds>(timeTaken)
            << "\n";
  std::cout << "Total samples: " << totalSamples << "\n";
  auto samplesPerSec =
      static_cast<double>(totalSamples)
      / std::chrono::duration_cast<std::chrono::milliseconds>(timeTaken)
            .count();
  std::cout << "Samples/ms: " << samplesPerSec << "\n";
}