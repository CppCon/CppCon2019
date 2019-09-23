#include "math/Vec3.h"

#include <benchmark/benchmark.h>

// All these tests carry a loop dependency to measure the _latency_

static void BM_Vec3LengthSquaredLatency(benchmark::State &state) {
  auto v = Vec3(1.2, -2.3, -12.4);
  for (auto _ : state) {
    v = Vec3(v.x() + v.lengthSquared(), v.y(), v.z());
  }
  benchmark::DoNotOptimize(v);
}

BENCHMARK(BM_Vec3LengthSquaredLatency);

static void BM_Vec3LengthLatency(benchmark::State &state) {
  auto v = Vec3(1.2, -2.3, -12.4);
  for (auto _ : state) {
    v = Vec3(v.x() + v.length(), v.y(), v.z());
  }
  benchmark::DoNotOptimize(v);
}

BENCHMARK(BM_Vec3LengthLatency);

static void BM_Vec3NormaliseLatency(benchmark::State &state) {
  auto v = Vec3(1.2, -2.3, -12.4);
  for (auto _ : state) {
    v += v.normalised().toVec3();
  }
  benchmark::DoNotOptimize(v);
}

BENCHMARK(BM_Vec3NormaliseLatency);

static void BM_Vec3DivideByScalarLatency(benchmark::State &state) {
  auto v = Vec3(1.2, -2.3, -12.4);
  for (auto _ : state) {
    auto divideBy(v.x() + v.y() + v.z());
    v += v / divideBy;
  }
  benchmark::DoNotOptimize(v);
}

BENCHMARK(BM_Vec3DivideByScalarLatency);

static void BM_Vec3DivideByScalarLatency_no_recip(benchmark::State &state) {
  auto v = Vec3(1.2, -2.3, -12.4);
  for (auto _ : state) {
    auto divideBy(v.x() + v.y() + v.z());
    v += Vec3(v.x() / divideBy, v.y() / divideBy, v.z() / divideBy);
  }
  benchmark::DoNotOptimize(v);
}

BENCHMARK(BM_Vec3DivideByScalarLatency_no_recip);

static void BM_Vec3DivideByScalarLatency_mul_recip(benchmark::State &state) {
  auto v = Vec3(1.2, -2.3, -12.4);
  for (auto _ : state) {
    auto divideBy(v.x() + v.y() + v.z());
    v += v * (1.0 / divideBy);
  }
  benchmark::DoNotOptimize(v);
}

BENCHMARK(BM_Vec3DivideByScalarLatency_mul_recip);

static void BM_Vec3AddLatency(benchmark::State &state) {
  auto v = Vec3(1.2, -2.3, -12.4);
  for (auto _ : state)
    v += v;
  benchmark::DoNotOptimize(v);
}

BENCHMARK(BM_Vec3AddLatency);

static void BM_Vec3MulLatency(benchmark::State &state) {
  auto v = Vec3(1.2, -2.3, -12.4);
  for (auto _ : state)
    v *= v;
  benchmark::DoNotOptimize(v);
}

BENCHMARK(BM_Vec3MulLatency);

static void BM_Vec3DotLatency_x2(benchmark::State &state) {
  auto v = Vec3(1.2, -2.3, -12.4);
  auto w = Vec3(0.2, 1.1, -0.1);
  for (auto _ : state) {
    w = Vec3(w.x(), w.y(), v.dot(w));
    v = Vec3(v.x(), v.y(), w.dot(v));
  }
  benchmark::DoNotOptimize(v);
  benchmark::DoNotOptimize(w);
}

BENCHMARK(BM_Vec3DotLatency_x2);

static void BM_Vec3CrossLatency_x2(benchmark::State &state) {
  auto v = Vec3(1.2, -2.3, -12.4);
  auto w = Vec3(0.2, 1.1, -0.1);
  for (auto _ : state) {
    w = w.cross(v);
    v = v.cross(w);
  }
  benchmark::DoNotOptimize(v);
  benchmark::DoNotOptimize(w);
}

BENCHMARK(BM_Vec3CrossLatency_x2);
