#  Makefile                                                      -*-makefile-*-
#  ----------------------------------------------------------------------------
#   Copyright (C) 2019 Dietmar Kuehl http://www.dietmar-kuehl.de         
#                                                                        
#   Permission is hereby granted, free of charge, to any person          
#   obtaining a copy of this software and associated documentation       
#   files (the "Software"), to deal in the Software without restriction, 
#   including without limitation the rights to use, copy, modify,        
#   merge, publish, distribute, sublicense, and/or sell copies of        
#   the Software, and to permit persons to whom the Software is          
#   furnished to do so, subject to the following conditions:             
#                                                                        
#   The above copyright notice and this permission notice shall be       
#   included in all copies or substantial portions of the Software.      
#                                                                        
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      
#   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      
#   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             
#   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          
#   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         
#   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        
#   OTHER DEALINGS IN THE SOFTWARE. 
#  ----------------------------------------------------------------------------

CXXFILES += executor2_inclusive_scan.t.cpp
CXXFILES += inclusive_scan.cpp
CXXFILES += benchmark.cpp

OFILES   = $(CXXFILES:%.cpp=%.o)
TSKFILES = $(CXXFILES:%.cpp=%.tsk)
RUNFILES = $(CXXFILES:%.cpp=%.run)

CC       = $(CXX)
CPPFLAGS = -std=c++17 -fopenmp
CPPFLAGS += -isystem /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include
CPPFLAGS += -I/opt/gcc-9.2.0/include
# SANITIZER = -fsanitize=thread
CXXFLAGS = -O3 -g $(SANITIZER)
CXXFLAGS +=  -DUSE_TBB -Wno-deprecated-declarations
LDFLAGS  += $(SANITIZER)
LDLIBS   = -fopenmp -lbenchmark -ltbb

INSTALL  = true
ifeq ($(shell uname -s),Darwin)
    INSTALL = install_name_tool 
endif

.PHONY: default
default: profile

.PHONY: test-is
test-is: inclusive_scan.run
	./inclusive_scan.run

.PHONY: csv
csv: benchmark.run
	./benchmark.run --benchmark_format=csv | tee results.csv

.PHONY: profile
profile: benchmark.run
	./benchmark.run --benchmark_out=profile.out --benchmark_out_format=csv

.PHONY: test
test: executor2_inclusive_scan.t.run
	./executor2_inclusive_scan.t.run

%.tsk: %.o
	$(CXX) -o $@ $(LDFLAGS) $(@:%.tsk=%.o) $(LDLIBS)

%.run: %.tsk
	cp $(@:%.run=%.tsk) $@
	$(INSTALL) -change @rpath/libtbb.dylib /opt/gcc-9.1.0/lib/libtbb.dylib $@

inclusive_scan.tsk: inclusive_scan
	cp inclusive_scan $@

benchmark.tsk: benchmark
	cp benchmark $@

.PHONY: clean
clean:
	$(RM) $(OFILES) $(TSKFILES) $(RUNFILES)
	$(RM) mkerr olderr *~ make.depend

.PHONY: depend
depend make.depend:
	$(CXX) $(CPPFLAGS) -M $(CXXFILES) > make.depend

include make.depend
