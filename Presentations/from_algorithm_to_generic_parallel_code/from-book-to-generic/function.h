// function.h                                                         -*-C++-*-
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

#ifndef INCLUDED_FUNCTION
#define INCLUDED_FUNCTION

#include <memory>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace nstd {
    template <typename> class function;
    template <typename RC, typename... A> class function<RC(A...)>;
}

// ----------------------------------------------------------------------------

template <typename RC, typename... A>
class nstd::function<RC(A...)> {
private:
    struct base {
        virtual ~base() = default;
        virtual RC call(A... a) = 0;
    };
    template <typename Fun>
    struct concrete
        : base {
        Fun d_fun;
        template <typename F>
        concrete(F&& fun): d_fun(std::forward<F>(fun)) {}
        concrete(concrete const&) =  delete;
        concrete(concrete&&) =  delete;
        RC call(A... a) {
            return this->d_fun(std::forward<A>(a)...);
        }
    };
    std::unique_ptr<base> d_fun;

public:
    function() = default;
    template <typename Fun>
    function(Fun&& fun): d_fun(std::make_unique<concrete<std::decay_t<Fun>>>(std::forward<Fun>(fun))) {}
    function(function const&) = delete;
    function(function&) = delete;
    function(function&& other): d_fun(std::move(other.d_fun)) {}
    explicit operator bool() const {return bool(this->d_fun); }
    RC operator()(A... a) const { return this->d_fun->call(std::forward<A>(a)...); }
};

// ----------------------------------------------------------------------------

#endif
