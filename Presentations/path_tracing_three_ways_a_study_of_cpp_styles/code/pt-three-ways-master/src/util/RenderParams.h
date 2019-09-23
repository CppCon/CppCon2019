#pragma once

struct RenderParams {
  int width{1920};
  int height{1080};
  bool preview{false};
  int samplesPerPixel{40};
  int maxCpus{1};
  int maxDepth{5};
  int firstBounceUSamples{4};
  int firstBounceVSamples{4};
  int seed{0};
};
