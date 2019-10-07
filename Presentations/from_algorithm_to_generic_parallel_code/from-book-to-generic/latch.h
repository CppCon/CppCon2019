// latch.hpp                                                          -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2017 Dietmar Kuehl http://www.dietmar-kuehl.de         
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

#ifndef INCLUDED_LATCH
#define INCLUDED_LATCH

#include <condition_variable>
#include <mutex>

// ----------------------------------------------------------------------------

namespace nstd {
    class latch;
    class latch_arriver;
}

// ----------------------------------------------------------------------------

class nstd::latch {
private:
    std::mutex              d_mutex;
    std::condition_variable d_condition;
    int                     d_await;

public:
    explicit latch(int await): d_await(await) {}
    void arrive() {
        std::lock_guard<std::mutex> kerberos(this->d_mutex);
        if (!--this->d_await) {
            this->d_condition.notify_one();
        }
    }
    void wait() {
        std::unique_lock<std::mutex> kerberos(this->d_mutex);
        this->d_condition.wait(kerberos, [this]{return !this->d_await; });
    }
};

// ----------------------------------------------------------------------------

class nstd::latch_arriver {
private:
    nstd::latch  *d_latch;
public:
    latch_arriver(nstd::latch& latch): d_latch(&latch) {}
    latch_arriver(nstd::latch_arriver&& other): d_latch(other.d_latch) { other.d_latch = nullptr; }
    latch_arriver(latch_arriver const&) = delete;
    ~latch_arriver() { if (this->d_latch) this->d_latch->arrive(); }
};

// ----------------------------------------------------------------------------

#endif
