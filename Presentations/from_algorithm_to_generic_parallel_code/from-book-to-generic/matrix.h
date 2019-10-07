// matrix.h                                                           -*-C++-*-
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

#ifndef INCLUDED_MATRIX
#define INCLUDED_MATRIX

// ----------------------------------------------------------------------------

namespace nstd {
    template <typename T,int N>
    class matrix {
    private:
        T  d_value[N][N];
    public:
        matrix():  d_value() {}
        explicit matrix(int diagonal)
            :  d_value() {
            for (int i = 0; i != N; ++i) {
                d_value[i][i] = diagonal;
            }
        }
        template<typename Distribution, typename Generator>
        matrix(Distribution& rnd, Generator& generator) {
            for (int i = 0; i != N; ++i) {
                for (int j = 0; j != N; ++j) {
                    d_value[i][j] = rnd(generator);
                }
            }
        }
        matrix& operator*= (matrix const& m) {
            return *this  = *this * m;
        }
        friend matrix operator*(matrix const& m0, matrix const& m1) {
            matrix rc;
            for (int i = 0; i != N; ++i) {
                for (int j = 0; j != N; ++j) {
                    T tmp{};
                    for (int k = 0; k != N; ++k) {
                        tmp += m0.d_value[i][k] * m1.d_value[k][j];
                    }
                    rc.d_value[i][j]  = tmp;
                }
            }
            return rc;
        }

        friend bool operator== (matrix const& m0, matrix const& m1) {
            for (int i = 0; i != N; ++i) {
                for (int j = 0; j != N; ++j) {
                    if (m0.d_value[i][j] != m1.d_value[i][j]) {
                        return false;
                    }
                }
            }
            return true;
        }
        friend bool operator!= (matrix const& m0, matrix const& m1) {
            return !(m0 == m1);
        }
    };
}

// ----------------------------------------------------------------------------

#endif
