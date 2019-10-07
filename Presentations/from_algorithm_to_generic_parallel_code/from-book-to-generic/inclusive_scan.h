// inclusive_scan.h                                                   -*-C++-*-
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

#ifndef INCLUDED_INCLUSIVE_SCAN
#define INCLUDED_INCLUSIVE_SCAN

#include "latch.h"
#ifdef USE_TBB
#include <tbb/task_group.h>
#include <tbb/parallel_for.h>
#endif
#include <algorithm>
#include <iostream>
#include <functional>
#include <future>
#include <iterator>
#include <numeric>
#include <optional>
#include <thread>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace config {
    constexpr ptrdiff_t chunk_size = 8192;
    constexpr ptrdiff_t chunk      = chunk_size;
}

namespace nstd {
    template <typename In, typename Out, typename Op, typename Value>
    Out inclusive_scan(In it, In end, Out to, Op op, Value value) {
        for (; it != end; ++it, ++to) {
            value = op(value, *it);
            *to = value;
        }
        return to;
    }

    template <typename In, typename Out, typename Op = std::plus<>>
    Out inclusive_scan(In it, In end, Out to, Op op = Op()) {
        if (it == end) {
            return to;
        }
        auto value(*it);
        *to = value;
        return inclusive_scan(++it, end, ++to, op, value);
    }
}

// ----------------------------------------------------------------------------
// --- close CLRS algorithms --------------------------------------------------
// ----------------------------------------------------------------------------

namespace clrs_original {
    // Very close to the original (but sequential).
    template <typename A, typename Op>
    typename A::value_type p_scan_up(A const& x, A& t, int i, int j, Op op) {
        if (i == j) {
            return x[i - 1];
        }
        else {
            auto k = (i + j) / 2;
            t[k - 1] = p_scan_up(x, t, i, k, op);
            auto right = p_scan_up(x, t, k + 1, j, op);
            return op(t[k - 1], right);
        }
    }

    template <typename A, typename Op>
    void p_scan_down(typename A::value_type v, A const& x, A const& t, A& y, int i, int j, Op op) {
        if (i == j) {
            y[i - 1] = op(v, x[i - 1]);
        }
        else {
            auto k = (i + j) / 2;
            p_scan_down(v, x, t, y, i, k, op);
            p_scan_down(op(v, t[k - 1]), x, t, y, k + 1, j, op);
        }
    }

    template <typename A, typename Op>
    A p_scan(A const& x, Op op) {
        A y(x.size());
        A t(x.size());

        y[0] = x[0];
        p_scan_up(x, t, 2, x.size(), op);
        p_scan_down(x[0], x, t, y, 2, x.size(), op);
        return y;
    }
}

// ----------------------------------------------------------------------------

namespace clrs_stl_seq {
    // STL interface, close to the original (still sequential).
    template <typename In, typename Tmp, typename Op>
    auto p_scan_up(In begin, In end, Tmp tmp, Op op) -> std::decay_t<decltype(*begin)> {
        auto n = std::distance(begin, end);
        if (1 == n) {
            return *begin;
        }
        else {
            auto k = n / 2;
            tmp[k] = p_scan_up(begin, begin + k, tmp, op);
            auto right = p_scan_up(begin + k, end, tmp + k, op);
            return op(tmp[k], right);
        }
    }

    template <typename Value, typename In, typename Tmp, typename To, typename Op>
    void p_scan_down(Value v, In begin, In end, Tmp tmp, To to, Op op) {
        auto n = std::distance(begin, end);
        if (1 == n) {
            *to = op(v, *begin);
        }
        else {
            auto k = n / 2;
            p_scan_down(v, begin, begin + k, tmp, to, op);
            p_scan_down(op(v, tmp[k]), begin + k, end, tmp + k, to + k, op);
        }
    }

    template <typename In, typename Out, typename Op>
    Out p_scan(In begin, In end, Out to, Op op) {
        auto n = std::distance(begin, end);
        if (0 < n) {
            to[0] = begin[0];
            if (1 < n) {
                std::vector<std::decay_t<decltype(*begin)>> t(n);
                p_scan_up(begin + 1, end, t.begin(), op);
                p_scan_down(begin[0], begin + 1, end, t.begin(), to + 1, op);
            }
        }
        return to + n;
    }
}

// ----------------------------------------------------------------------------

namespace clrs_async {
    // STL interface, close to the original, parallel via tbb::task_group
    template <typename In, typename Tmp, typename Op>
    auto p_scan_up(In begin, In end, Tmp tmp, Op op) -> std::decay_t<decltype(*begin)> {
        auto n = std::distance(begin, end);
        if (1 == n) {
            return *begin;
        }
        else {
            auto k = n / 2;
            auto fut = std::async([&]{
                    tmp[k] = p_scan_up(begin, begin + k, tmp, op);
                });
            auto right = p_scan_up(begin + k, end, tmp + k, op);
            fut.wait();
            return op(tmp[k], right);
        }
    }

    template <typename Value, typename In, typename Tmp, typename To, typename Op>
    void p_scan_down(Value v, In begin, In end, Tmp tmp, To to, Op op) {
        auto n = std::distance(begin, end);
        if (1 == n) {
            *to = op(v, *begin);
        }
        else {
            auto k = n / 2;
            auto fut = std::async([&]{
                    p_scan_down(v, begin, begin + k, tmp, to, op);
                });
            p_scan_down(op(v, tmp[k]), begin + k, end, tmp + k, to + k, op);
            fut.wait();
        }
    }

    template <typename In, typename Out, typename Op>
    Out p_scan(In begin, In end, Out to, Op op) {
        auto n = std::distance(begin, end);
        if (0 < n) {
            to[0] = begin[0];
            if (1 < n) {
                std::vector<std::decay_t<decltype(*begin)>> t(n);
                p_scan_up(begin + 1, end, t.begin(), op);
                p_scan_down(begin[0], begin + 1, end, t.begin(), to + 1, op);
            }
        }
        return to + n;
    }
}

// ----------------------------------------------------------------------------

namespace clrs_async_block {
    // STL interface, parallel via tbb::task_group, stop recursion at a random size

    template <int PScanMinSize, typename In, typename Tmp, typename Op>
    auto p_scan_up(In begin, In end, Tmp tmp, Op op) -> std::decay_t<decltype(*begin)> {
        auto n = std::distance(begin, end);
        if (n < PScanMinSize) {
            return std::accumulate(begin + 1, end, *begin, op);
        }
        else {
            auto k = n / 2;
            auto fut = std::async([&]{
                    tmp[k] = p_scan_up<PScanMinSize>(begin, begin + k, tmp, op);
                });
            auto right = p_scan_up<PScanMinSize>(begin + k, end, tmp + k, op);
            fut.wait();
            return op(tmp[k], right);
        }
    }

    template <int PScanMinSize, typename Value, typename In, typename Tmp, typename To, typename Op>
    void p_scan_down(Value v, In begin, In end, Tmp tmp, To to, Op op) {
        auto n = std::distance(begin, end);
        if (n < PScanMinSize) {
            nstd::inclusive_scan(begin, end, to, op, v);
        }
        else {
            auto k = n / 2;
            tbb::task_group group;
            auto fut = std::async([&]{
                    p_scan_down<PScanMinSize>(v, begin, begin + k, tmp, to, op);
                });
            p_scan_down<PScanMinSize>(op(v, tmp[k]), begin + k, end, tmp + k, to + k, op);
            fut.wait();
        }
    }

    template <int PScanMinSize, typename In, typename Out, typename Op>
    Out p_scan(In begin, In end, Out to, Op op) {
        auto n = std::distance(begin, end);
        if (0 < n) {
            to[0] = begin[0];
            if (1 < n) {
                std::vector<std::decay_t<decltype(*begin)>> t(n);
                p_scan_up<PScanMinSize>(begin + 1, end, t.begin(), op);
                p_scan_down<PScanMinSize>(begin[0], begin + 1, end, t.begin(), to + 1, op);
            }
        }
        return to + n;
    }
}

// ----------------------------------------------------------------------------

namespace clrs_par {
    // STL interface, close to the original, parallel via tbb::task_group
    template <typename In, typename Tmp, typename Op>
    auto p_scan_up(In begin, In end, Tmp tmp, Op op) -> std::decay_t<decltype(*begin)> {
        auto n = std::distance(begin, end);
        if (1 == n) {
            return *begin;
        }
        else {
            auto k = n / 2;
            tbb::task_group group;
            group.run([&]{
                    tmp[k] = p_scan_up(begin, begin + k, tmp, op);
                });
            auto right = p_scan_up(begin + k, end, tmp + k, op);
            group.wait();
            return op(tmp[k], right);
        }
    }

    template <typename Value, typename In, typename Tmp, typename To, typename Op>
    void p_scan_down(Value v, In begin, In end, Tmp tmp, To to, Op op) {
        auto n = std::distance(begin, end);
        if (1 == n) {
            *to = op(v, *begin);
        }
        else {
            auto k = n / 2;
            tbb::task_group group;
            group.run([&]{
                    p_scan_down(v, begin, begin + k, tmp, to, op);
                });
            p_scan_down(op(v, tmp[k]), begin + k, end, tmp + k, to + k, op);
            group.wait();
        }
    }

    template <typename In, typename Out, typename Op>
    Out p_scan(In begin, In end, Out to, Op op) {
        auto n = std::distance(begin, end);
        if (0 < n) {
            to[0] = begin[0];
            if (1 < n) {
                std::vector<std::decay_t<decltype(*begin)>> t(n);
                p_scan_up(begin + 1, end, t.begin(), op);
                p_scan_down(begin[0], begin + 1, end, t.begin(), to + 1, op);
            }
        }
        return to + n;
    }
}

// ----------------------------------------------------------------------------

namespace clrs_par_block {
    // STL interface, parallel via tbb::task_group, stop recursion at a random size

    template <int PScanMinSize, typename In, typename Tmp, typename Op>
    auto p_scan_up(In begin, In end, Tmp tmp, Op op) -> std::decay_t<decltype(*begin)> {
        auto n = std::distance(begin, end);
        if (n < PScanMinSize) {
            return std::accumulate(begin + 1, end, *begin, op);
        }
        else {
            auto k = n / 2;
            tbb::task_group group;
            group.run([&]{
                    tmp[k] = p_scan_up<PScanMinSize>(begin, begin + k, tmp, op);
                });
            auto right = p_scan_up<PScanMinSize>(begin + k, end, tmp + k, op);
            group.wait();
            return op(tmp[k], right);
        }
    }

    template <int PScanMinSize, typename Value, typename In, typename Tmp, typename To, typename Op>
    void p_scan_down(Value v, In begin, In end, Tmp tmp, To to, Op op) {
        auto n = std::distance(begin, end);
        if (n < PScanMinSize) {
            nstd::inclusive_scan(begin, end, to, op, v);
        }
        else {
            auto k = n / 2;
            tbb::task_group group;
            group.run([&]{
                    p_scan_down<PScanMinSize>(v, begin, begin + k, tmp, to, op);
                });
            p_scan_down<PScanMinSize>(op(v, tmp[k]), begin + k, end, tmp + k, to + k, op);
            group.wait();
        }
    }

    template <int PScanMinSize, typename In, typename Out, typename Op>
    Out p_scan(In begin, In end, Out to, Op op) {
        auto n = std::distance(begin, end);
        if (0 < n) {
            to[0] = begin[0];
            if (1 < n) {
                std::vector<std::decay_t<decltype(*begin)>> t(n);
                p_scan_up<PScanMinSize>(begin + 1, end, t.begin(), op);
                p_scan_down<PScanMinSize>(begin[0], begin + 1, end, t.begin(), to + 1, op);
            }
        }
        return to + n;
    }
}

// ----------------------------------------------------------------------------

namespace seq {
    template <typename InIt, typename OutIt, typename Op = std::plus<>>
    OutIt inclusive_scan(InIt begin, InIt end, OutIt to, Op op = Op()) {
        using ptrdiff_t = typename std::iterator_traits<InIt>::difference_type;
        using value_type = typename std::iterator_traits<InIt>::value_type;
        constexpr ptrdiff_t chunk_size = config::chunk_size;

        ptrdiff_t const size = std::distance(begin, end);
        if (size < chunk_size) {
            return nstd::inclusive_scan(begin, end, to, op);
        }

        ptrdiff_t const chunk_count = (size + chunk_size - 1) / chunk_size;
        std::vector<std::optional<value_type>> intermediate(chunk_count - 1);

        for (ptrdiff_t i(0); i != chunk_count - 1; ++i) {
            // the first element of each chunk is used as the initial value
            intermediate[i] = std::accumulate(begin + chunk_size * i + 1,
                                              begin + chunk_size * (i + 1),
                                              begin[chunk_size * i],
                                              op);
        }
        nstd::inclusive_scan(intermediate.begin(), intermediate.end(),
                             intermediate.begin(),
                             [op](auto& a, auto& b){ return op(*a, *b); });


        nstd::inclusive_scan(begin, begin + chunk_size, to, op);
        for (ptrdiff_t i = 1; i != chunk_count - 1;  ++i) {
            nstd::inclusive_scan(begin + chunk_size * i, begin + chunk_size * (i + 1),
                                 to + chunk_size * i, op, *intermediate[i - 1]);
        }
        return nstd::inclusive_scan(begin + chunk_size * (chunk_count - 1), end,
                                    to + chunk_size * (chunk_count - 1),
                                    op, *intermediate.back());
    }
}

// ----------------------------------------------------------------------------

namespace omp {
    template <typename InIt, typename OutIt, typename Op = std::plus<>>
    OutIt inclusive_scan(InIt begin, InIt end, OutIt to, Op op = Op()) {
        using ptrdiff_t = typename std::iterator_traits<InIt>::difference_type;
        using value_type = typename std::iterator_traits<InIt>::value_type;

        ptrdiff_t const size = std::distance(begin, end);
        ptrdiff_t chunk_size = size / std::thread::hardware_concurrency() / 4;
        if (chunk_size < 8) {
            return nstd::inclusive_scan(begin, end, to, op);
        }

        ptrdiff_t const chunk_count = (size + chunk_size - 1) / chunk_size;
        std::vector<std::optional<value_type>> intermediate(chunk_count - 1);

        #pragma omp parallel for
        for (ptrdiff_t i = 0; i != chunk_count - 1; ++i) {
            // the first element of each chunk is used as the initial value
            intermediate[i] = std::accumulate(begin + chunk_size * i + 1,
                                              begin + chunk_size * (i + 1),
                                              begin[chunk_size * i],
                                              op);
        }
        nstd::inclusive_scan(intermediate.begin(), intermediate.end(),
                             intermediate.begin(),
                             [op](auto& a, auto& b){ return op(*a, *b); });


        nstd::inclusive_scan(begin, begin + chunk_size, to, op);
        #pragma omp parallel for
        for (ptrdiff_t i = 1; i != chunk_count - 1;  ++i) {
            nstd::inclusive_scan(begin + chunk_size * i, begin + chunk_size * (i + 1),
                                 to + chunk_size * i, op, *intermediate[i - 1]);
        }
        return nstd::inclusive_scan(begin + chunk_size * (chunk_count - 1), end,
                                    to + chunk_size * (chunk_count - 1),
                                    op, *intermediate.back());
    }
}

// ----------------------------------------------------------------------------

namespace omp_all_par {
    template <typename InIt, typename OutIt, typename Op = std::plus<>>
    OutIt inclusive_scan(InIt begin, InIt end, OutIt to, Op op = Op()) {
        using ptrdiff_t = typename std::iterator_traits<InIt>::difference_type;
        using value_type = std::decay_t<decltype(op(*begin, *begin))>;

        ptrdiff_t const chunk = config::chunk;
        ptrdiff_t const size = std::distance(begin, end);
        ptrdiff_t const count = (size + chunk - 1) / chunk;
        std::vector<std::optional<value_type>> tmp(count);

        #pragma omp parallel for
        for (ptrdiff_t i = 0; i != count - 1; ++i) {
            tmp[i] = std::accumulate(begin + chunk * i + 1, begin + chunk * (i + 1),
                                     begin[chunk * i],
                                     op);
        }

        std::partial_sum(tmp.begin(), tmp.end(), tmp.begin(),
                         [op](auto& a, auto& b){ return op(*a, *b); });

        #pragma omp parallel for
        for (ptrdiff_t i = 0; i < count; ++i) {
            auto it = begin + chunk * i;
            auto value = i? op(*tmp[i - 1], *it): *it;
            to[it - begin] = value;
            nstd::inclusive_scan(it + 1, it + std::min(end - it, chunk),
                                 to + (it - begin) + 1, op, value);
        }
        return to + size;
    }
}

// ----------------------------------------------------------------------------

namespace tbb {
    template <typename InIt, typename OutIt, typename Op = std::plus<>>
    OutIt inclusive_scan(InIt begin, InIt end, OutIt to, Op op = Op()) {
        using ptrdiff_t = typename std::iterator_traits<InIt>::difference_type;
        using value_type = typename std::iterator_traits<InIt>::value_type;

        ptrdiff_t const size = std::distance(begin, end);
        ptrdiff_t chunk_size = size / std::thread::hardware_concurrency() / 4;
	chunk_size = std::max(ptrdiff_t(1024), chunk_size);

        ptrdiff_t const chunk_count = (size + chunk_size - 1) / chunk_size;
        if (chunk_count == 1) {
            return nstd::inclusive_scan(begin, end, to, op);
        }
        std::vector<std::optional<value_type>> intermediate(chunk_count - 1);

	tbb::task_group group;
        for (ptrdiff_t i = 0; i != chunk_count - 1; ++i) {
            // the first element of each chunk is used as the initial value
            group.run([&, i]{
                intermediate[i] = std::accumulate(begin + chunk_size * i + 1,
                                                  begin + chunk_size * (i + 1),
                                                  begin[chunk_size * i],
                                                  op);
            });
        }
        group.wait();
        nstd::inclusive_scan(intermediate.begin(), intermediate.end(),
                             intermediate.begin(),
                             [op](auto& a, auto& b){ return op(*a, *b); });


	group.run([&]{
            nstd::inclusive_scan(begin, begin + chunk_size, to, op);
        });
        for (ptrdiff_t i = 1; i != chunk_count - 1;  ++i) {
            group.run([&, i]{
                nstd::inclusive_scan(begin + chunk_size * i, begin + chunk_size * (i + 1),
                                     to + chunk_size * i, op, *intermediate[i - 1]);
            });
        }
        group.run([&]{
            nstd::inclusive_scan(begin + chunk_size * (chunk_count - 1), end,
                                 to + chunk_size * (chunk_count - 1),
                                 op, *intermediate.back());
        });
	group.wait();
        return to + size;
    }
}


namespace tbb_special {
    template <typename InIt, typename OutIt, typename Op = std::plus<>>
    OutIt inclusive_scan(InIt begin, InIt end, OutIt to, Op op = Op()) {
        using ptrdiff_t = typename std::iterator_traits<InIt>::difference_type;
        using value_type = typename std::iterator_traits<InIt>::value_type;

        ptrdiff_t const size = std::distance(begin, end);
        ptrdiff_t chunk_size = size / std::thread::hardware_concurrency() / 4;

        if (chunk_size < 8) {
            return nstd::inclusive_scan(begin, end, to, op);
        }

        ptrdiff_t const chunk_count = (size + chunk_size - 1) / chunk_size;
        std::vector<std::optional<value_type>> intermediate(chunk_count - 1);

        {
            tbb::task_group group;
            group.run([&]{
                    nstd::inclusive_scan(begin, begin + chunk_size, to, op);
                    intermediate[0] = to[chunk_size - 1];
                });
            for (ptrdiff_t i = 1; i != chunk_count - 1; ++i) {
                group.run([&, i]{
                        // the first element of each chunk is used as the initial value
                        intermediate[i] = std::accumulate(begin + chunk_size * i + 1,
                                                          begin + chunk_size * (i + 1),
                                                          begin[chunk_size * i],
                                                          op);
                    });
            }
            group.wait();
        }
        
        nstd::inclusive_scan(intermediate.begin(), intermediate.end(),
                             intermediate.begin(),
                             [op](auto& a, auto& b){ return op(*a, *b); });

        {
            tbb::task_group group;
            for (ptrdiff_t i = 1; i != chunk_count - 1;  ++i) {
                group.run([&, i]{
                        nstd::inclusive_scan(begin + chunk_size * i, begin + chunk_size * (i + 1),
                                             to + chunk_size * i, op, *intermediate[i - 1]);
                    });
            }
            group.run([&]{
                    nstd::inclusive_scan(begin + chunk_size * (chunk_count - 1), end,
                                         to + chunk_size * (chunk_count - 1),
                                         op, *intermediate.back());
                });
            group.wait();
        }
        return to + size;
    }
}
// ----------------------------------------------------------------------------

namespace tbb_all_par {
    template <typename InIt, typename OutIt, typename Op = std::plus<>>
    OutIt inclusive_scan(InIt begin, InIt end, OutIt to, Op op = Op()) {
        using ptrdiff_t = typename std::iterator_traits<InIt>::difference_type;
        using value_type = std::decay_t<decltype(op(*begin, *begin))>;

        ptrdiff_t const chunk = config::chunk;
        ptrdiff_t const size = std::distance(begin, end);
        ptrdiff_t const count = (size + chunk - 1) / chunk;
        std::vector<std::optional<value_type>> tmp(count);

	tbb::task_group group;
        for (ptrdiff_t i = 0; i != count - 1; ++i) {
            group.run([&, i]{
                    tmp[i] = std::accumulate(begin + chunk * i + 1, begin + chunk * (i + 1),
                                             begin[chunk * i], op);
                });
        }
        group.wait();

        std::partial_sum(tmp.begin(), tmp.end(), tmp.begin(),
                         [op](auto& a, auto& b){ return op(*a, *b); });

        for (ptrdiff_t i = 0; i < count; ++i) {
            group.run([&, i]{
                    auto it = begin + chunk * i;
                    auto value = i? op(*tmp[i - 1], *it): *it;
                    nstd::inclusive_scan(it + 1, it + std::min(end - it, chunk),
                                         to + (it - begin) + 1, op, to[it - begin] = value);
                });
        }
	group.wait();
        return to + size;
    }
}

namespace tbb_par_for {
    template <typename InIt, typename OutIt, typename Op = std::plus<>>
    OutIt inclusive_scan(InIt begin, InIt end, OutIt to, Op op = Op()) {
        using ptrdiff_t = typename std::iterator_traits<InIt>::difference_type;
        using value_type = std::decay_t<decltype(op(*begin, *begin))>;

        ptrdiff_t const chunk = config::chunk;
        ptrdiff_t const size = std::distance(begin, end);
        ptrdiff_t const count = (size + chunk - 1) / chunk;
        std::vector<std::optional<value_type>> tmp(count);

        tbb::parallel_for(ptrdiff_t(), count - 1, 
                          [&](auto i) {
                              tmp[i] = std::accumulate(begin + chunk * i + 1, begin + chunk * (i + 1),
                                                       begin[chunk * i], op);
                });

        std::partial_sum(tmp.begin(), tmp.end(), tmp.begin(),
                         [op](auto& a, auto& b){ return op(*a, *b); });

        tbb::parallel_for(ptrdiff_t(), count, 
                          [&](auto i) {
                              auto it = begin + chunk * i;
                              auto value = i? op(*tmp[i - 1], *it): *it;
                              nstd::inclusive_scan(it + 1, it + std::min(end - it, chunk),
                                                   to + (it - begin) + 1, op, to[it - begin] = value);
                          });
        return to + size;
    }
}

// ----------------------------------------------------------------------------

namespace executor {
    template <typename Executor, typename Size, typename InIt, typename Fun>
    void for_each_subrange(Executor exec, Size chunk, InIt begin, InIt end, Fun fun) {
        if (begin == end) {
            return;
        }
        ptrdiff_t const size = std::distance(begin, end);
        ptrdiff_t const count = (size + chunk - 1) / chunk;
        nstd::latch  latch(count);
        auto it(begin);
        for (ptrdiff_t i(0); i != count - 1; ++i) {
            auto cend(std::next(it, chunk));
            exec.execute([&, i, it, cend, arriver = nstd::latch_arriver(latch)]{
                    fun(i, it, cend);
                });
            it = cend;
        }
        exec.execute([&, it, arriver = nstd::latch_arriver(latch)]{
                fun(count - 1, it, end);
            });
        latch.wait();
    }

    template <typename Executor, typename InIt, typename OutIt, typename Op = std::plus<>>
    OutIt inclusive_scan(Executor exec, InIt begin, InIt end, OutIt to, Op op = Op()) {
        using ptrdiff_t = typename std::iterator_traits<InIt>::difference_type;
        using value_type = std::decay_t<decltype(op(*begin, *begin))>;

        ptrdiff_t const size = std::distance(begin, end);
        ptrdiff_t const chunk = std::max(ptrdiff_t(8), ptrdiff_t(size / std::thread::hardware_concurrency()));
        ptrdiff_t const count = (size + chunk - 1) / chunk;
        std::vector<std::optional<value_type>> tmp(count +  1);

        executor::for_each_subrange(exec, chunk, begin, end,
            [&](auto i, auto begin, auto end) {
                tmp[i + 1] = std::accumulate(std::next(begin), end, *begin, op);
            });
        std::partial_sum(tmp.begin(), tmp.end(), tmp.begin(),
                            [op](auto& a, auto& b){ return op(*a, *b); });

        executor::for_each_subrange(exec, chunk, begin, end,
            [&](auto i,  auto sbegin, auto send) {
                auto value = i? op(*tmp[i], *sbegin): *sbegin;
                auto offset(std::distance(begin, sbegin));
                nstd::inclusive_scan(std::next(sbegin), send,
                                     to + offset + 1,
                                     op, to[offset] = value);
            });
        return to + size;
    }
}

// ----------------------------------------------------------------------------

#endif
