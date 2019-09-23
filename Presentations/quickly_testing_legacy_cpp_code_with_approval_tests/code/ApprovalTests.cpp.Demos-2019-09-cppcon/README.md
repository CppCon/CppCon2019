# ApprovalTests.cpp Demo code

## Introduction

Code samples and information for various talks and demos on ApprovalTests.cpp.

The samples here demonstrate use of [ApprovalTests.cpp](https://github.com/approvals/ApprovalTests.cpp), which is the C++ implementation of Llewellyn Falco's [Approval Tests](http://approvaltests.com/) approach to testing legacy and hard-to-test code.

## Branches

This evolving repo contains demos used in multiple different talks over time.

There will be different branches for each successive generation of the code, differing in things like which versions of [ApprovalTests.cpp and other 3rd-party libraries](third_party/) are used. 

* [master](https://github.com/claremacrae/ApprovalTests.cpp.Demos/tree/master)
* [2019-09-accu-cambridge](https://github.com/claremacrae/ApprovalTests.cpp.Demos/tree/2019-09-accu-cambridge)
* [2019-09-cppcon](https://github.com/claremacrae/ApprovalTests.cpp.Demos/tree/2019-09-cppcon)

## Talks and presentations

For more information, see my list of [talks and presentations](https://claremacrae.co.uk/conferences/presentations.html), with links to any slides and recordings.

## ApprovalTests.cpp code samples

Purpose of the sub-directories in this repo:

* [catch2_demo/](catch2_demo)
    * Some example uses of [ApprovalTests.cpp](https://github.com/approvals/ApprovalTests.cpp) with the [Catch2 test framework](https://github.com/catchorg/Catch2), with detailed explanatory comments in the code.
* [google_test_demo/](google_test_demo)
    * Some example uses of [ApprovalTests.cpp](https://github.com/approvals/ApprovalTests.cpp) with the [Google Test framework](https://github.com/google/googletest), with detailed explanatory comments in the code.
* [catch1_demo/](catch1_demo)
    * Minimal example for now, which shows how to set up the main for use with [Catch1](https://github.com/catchorg/Catch2/tree/Catch1.x)
* [doctest_demo/](doctest_demo)
    * Minimal example for now, which shows how to set up the main for use with [doctest](https://github.com/onqtam/doctest)
* [include](include)
    * Some code used by the above demos (all files inlined for now, for simplicity)

## Samples elsewhere

* [gilded_rose_refactoring_kata](https://github.com/claremacrae/cppp2019/tree/master/gilded_rose_refactoring_kata)
    * A worked example of the [cpp version of Emily Bache's GildedRose Refactoring Kata](https://github.com/emilybache/GildedRose-Refactoring-Kata/tree/master/cpp)
* The [ApprovalTests.cpp User Guide](https://github.com/approvals/ApprovalTests.cpp/blob/master/doc/README.md#top) also has plenty of code snippets showing how to use the library.
