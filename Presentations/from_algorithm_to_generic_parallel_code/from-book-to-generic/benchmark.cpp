// benchmark.cpp                                                      -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2019 Dietmar Kuehl http://www.dietmar-kuehl.de         
//                                                                       
//  Permission is hereby granted, free of charge, to any person          
//  obtaining a copy of this software and associated documentation       
//  files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify,        
//  merge, publish, distribute, sublicense, and/or sell copies of        
//  the Software, and to permit persons to whom the Software is          
//  furnished to do so, subject to the following conditions:             
//                                                                       
//  The above copyright notice and this permission notice shall be       
//  included in all copies or substantial portions of the Software.      
//                                                                       
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        
//  OTHER DEALINGS IN THE SOFTWARE. 
// ----------------------------------------------------------------------------

#include "tbb/parallel_scan.h"
#include "inclusive_scan.h"
#include "executor.h"
#include "matrix.h"
#include <benchmark/benchmark.h>
#include <execution>
#include <functional>
#include <numeric>
#include <random>
#include <vector>

constexpr std::size_t size = 100000000;

// ----------------------------------------------------------------------------

namespace {
    void test(std::initializer_list<int> input)
    {
        std::vector<int> result1(input.size());
        auto it1 = std::partial_sum(input.begin(), input.end(), result1.begin(), std::plus<>());
        
    }
}

// ----------------------------------------------------------------------------

namespace {
    template <typename Algo>
    void bench1(benchmark::State& state, Algo algo) {
        std::size_t size = state.range(0) * 10000;
        std::vector<unsigned long long> input;
        std::generate_n(std::back_inserter(input), size, [i = 0]()mutable { return ++i; });
        std::vector<unsigned long long> output(input.size());

        for (auto _: state) {
            benchmark::DoNotOptimize(algo(input, output, std::plus<>()));
        }
    }

    template <typename T> struct make_value_t;
    template <typename T, int Size>
    struct make_value_t<nstd::matrix<T, Size>>
    {
        template <typename Rnd, typename Gen>
        nstd::matrix<T, Size> operator()(Rnd& rnd, Gen& gen) const {
            return nstd::matrix<T, Size>(rnd, gen);
        }
    };
    template <>
    struct make_value_t<unsigned long>
    {
        template <typename Rnd, typename Gen>
        unsigned long operator()(Rnd& rnd, Gen& gen) const {
            return rnd(gen);
        }
    };
    template <typename T>
    inline constexpr make_value_t<T> make_value;
    
    template <typename Algo>
    void bench(benchmark::State& state, Algo algo) {
        std::size_t size = state.range(0) * 100;

        {
            std::vector<unsigned long long> input;
            std::generate_n(std::back_inserter(input), size,
                            [i = 0ull]()mutable { return ++i; });
            std::vector<unsigned long long> output(input.size());
            std::vector<unsigned long long> result(input.size());
            std::partial_sum(input.begin(), input.end(),
                             result.begin(), std::plus<>());
            algo(input, output, std::plus<>(), 0ull);
            if (output != result) {
                std::cout << "results differ!\n";
            }
        }
        {
//#define USE_INT
#ifdef USE_INT
            using value_type = unsigned long;
            using op_type    = std::plus<>;
            std::uniform_int_distribution<value_type> rnd(0, 16384);
            value_type                                neutral(0);
#else
            using value_type = nstd::matrix<float, 8>;
            using op_type    = std::multiplies<>;
            std::uniform_real_distribution<float> rnd(-1.0, 1.0);
            value_type                            neutral(1);
#endif

            std::vector<value_type> input;
            std::minstd_rand        gen;
            std::generate_n(std::back_inserter(input), size,
                            [&]()mutable { return make_value<value_type>(rnd, gen); });
            std::vector<value_type> output(input.size());
        
            for (auto _: state) {
                benchmark::DoNotOptimize(algo(input, output, op_type(), neutral));
            }
        }
    }
}

#define RUN_ALL_BENCHMARKS 
#ifdef RUN_ALL_BENCHMARKS 
int constexpr RM = 2;

#define RUN_STD_BENCHMARKS
#define RUN_CLRS_BENCHMARKS
#define RUN_OMP_BENCHMARKS
#define RUN_TBB_BENCHMARKS
#define RUN_EXECUTOR_BENCHMARKS
#else
int constexpr RM = 8;
#endif

// Run the sequential version from the C++ standard library.
BENCHMARK_CAPTURE(bench,
                  std_classic,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return std::partial_sum(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

//#define RUN_STD_BENCHMARKS
#ifdef RUN_STD_BENCHMARKS
// Run the parallel versions from the C++ standard library.
BENCHMARK_CAPTURE(bench,
                  std_seq,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return std::inclusive_scan(std::execution::seq,
                                                 x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  std_par,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return std::inclusive_scan(std::execution::par,
                                                 x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  std_par_unseq,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return std::inclusive_scan(std::execution::par_unseq,
                                                 x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();
#endif

//#define RUN_CLRS_BENCHMARKS
#ifdef RUN_CLRS_BENCHMARKS

BENCHMARK_CAPTURE(bench,
                  clrs_original,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      o = clrs_original::p_scan(x, op);
                      return o.end();
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  clrs_stl_seq,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return clrs_stl_seq::p_scan(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  clrs_async,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      if (std::distance(x.begin(), x.end()) < 60000) {
                          return clrs_async::p_scan(x.begin(), x.end(), o.begin(), op);
                      }
                      else {
                          return std::partial_sum(x.begin(), x.end(), o.begin(), op);
                      }
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  clrs_async_block_64,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return clrs_async_block::p_scan<64>(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  clrs_async_block_128,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return clrs_async_block::p_scan<128>(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  clrs_async_block_256,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return clrs_async_block::p_scan<256>(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  clrs_par,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return clrs_par::p_scan(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  clrs_par_block_64,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return clrs_par_block::p_scan<64>(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  clrs_par_block_128,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return clrs_par_block::p_scan<128>(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  clrs_par_block_256,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return clrs_par_block::p_scan<256>(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();
#endif

// #define RUN_OMP_BENCHMARKS
#ifdef RUN_OMP_BENCHMARKS
// Run benchmarks for implementations using OpenMP

BENCHMARK_CAPTURE(bench,
                  seq,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return seq::inclusive_scan(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  omp,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return omp::inclusive_scan(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  omp_all_par,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return omp_all_par::inclusive_scan(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

#endif

//#define RUN_TBB_BENCHMARKS
#ifdef RUN_TBB_BENCHMARKS
// Run benchmarks for TBB based versions

BENCHMARK_CAPTURE(bench,
                  tbb_parallel_scan,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      using value_type = std::decay_t<decltype(x[0])>;
                      return tbb::parallel_scan(tbb::blocked_range<int>(0, x.size()), 
                                                neutral, 
                                                [&](tbb::blocked_range<int> const& r, value_type sum, bool is_final)->value_type {
                                                    if (is_final) {
                                                        nstd::inclusive_scan(x.begin() + r.begin(),
                                                                             x.begin() + r.end(),
                                                                             o.begin() + r.begin(),
                                                                             op,
                                                                             sum);
                                                        return o[r.end() - 1];
                                                    }
                                                    else {
                                                        return std::accumulate(x.begin() + r.begin(),
                                                                               x.begin() + r.end(),
                                                                               sum,
                                                                               op);
                                                    }
                                                },
                                                [&](auto&& left, auto&& right) {
                                                    return op(left, right);
                                                }
                                                 );
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  tbb,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return tbb::inclusive_scan(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  tbb_special, // treat first/last chunk special but within the group
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return tbb_special::inclusive_scan(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  tbb_all_par, // treat first/last chunk special but within the group
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return tbb_all_par::inclusive_scan(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  tbb_par_for, // treat first/last chunk special but within the group
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      return tbb_par_for::inclusive_scan(x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();
#endif

//#define RUN_EXECUTOR_BENCHMARKS
#ifdef RUN_EXECUTOR_BENCHMARKS

BENCHMARK_CAPTURE(bench,
                  executor_seq,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      auto executor = []{ static nstd::immediate_executor rc; return rc; }();
                      return executor::inclusive_scan(executor, x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  executor_pool,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      auto executor = []{ static nstd::pool_executor rc; return rc; }();
                      return executor::inclusive_scan(executor, x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  executor_multipool,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      auto executor = []{ static nstd::multipool_executor rc; return rc; }();
                      return executor::inclusive_scan(executor, x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  executor_job_stealing,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      auto executor = []{ static nstd::job_stealing_executor rc; return rc; }();
                      return executor::inclusive_scan(executor, x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();

BENCHMARK_CAPTURE(bench,
                  executor_tbb,
                  [](auto const& x, auto& o, auto op, auto neutral) {
                      auto executor = []{ static nstd::tbb_executor rc; return rc; }();
                      return executor::inclusive_scan(executor, x.begin(), x.end(), o.begin(), op);
                  })->RangeMultiplier(RM)->Range(8, 8 << 10)->UseRealTime();
#endif

BENCHMARK_MAIN();
