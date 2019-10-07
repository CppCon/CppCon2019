// inclusive_scan.cpp                                                 -*-C++-*-
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

#include "inclusive_scan.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

// ----------------------------------------------------------------------------

namespace {
    template <typename T>
    struct format {
        std::vector<T> const& d_v;
        format(std::vector<T> const& v): d_v(v) {}
        friend std::ostream& operator<< (std::ostream& out, format const& f) {
            for (auto const& v: f.d_v) {
                out << " " << v;
            }
            return out;
        }
    };
}

// ----------------------------------------------------------------------------

int main() {
    std::vector<int> const x{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::vector<int> e(x.size());
    std::vector<int> n(x.size());
    std::vector<int> z(x.size());
    std::vector<int> s(x.size());
    std::vector<int> o(x.size());
    std::vector<int> t(x.size());
    std::vector<int> exec(x.size());
    int              executor;

    std::cout << std::thread::hardware_concurrency() << "\n" << std::flush;
    std::partial_sum(x.begin(), x.end(), e.begin());
    nstd::inclusive_scan(x.begin(), x.end(), n.begin());
    auto y(clrs::p_scan(x));
    clrs2::p_scan(x.begin(), x.end(), z.begin());
    sequential::inclusive_scan(x.begin(), x.end(), s.begin());
    omp::inclusive_scan(x.begin(), x.end(), o.begin());
    mtbb::inclusive_scan(x.begin(), x.end(), t.begin());
    executor::inclusive_scan(executor, x.begin(), x.end(), exec.begin());

    std::cout << "x=" << format(x) << "\n"
              << "e=" << format(e) << "\n"
              << "n=" << format(n) << "\n"
              << "y=" << format(y) << "\n"
              << "z=" << format(z) << "\n"
              << "s=" << format(s) << "\n"
              << "o=" << format(o) << "\n"
              << "t=" << format(t) << "\n"
              << "x=" << format(exec) << "\n"
        ;
}
