cmake_minimum_required(VERSION 3.12)
project(pt_three_ways)

find_program(CONAN conan)
if (NOT CONAN)
    message(FATAL_ERROR "Needs conan package manager: please visit https://conan.io and install")
endif ()

include(${CMAKE_SOURCE_DIR}/scripts/conan.cmake)

conan_check(VERSION 1.14.4 REQUIRED)
conan_add_remote(NAME bincrafters INDEX 1
        URL https://api.bintray.com/conan/bincrafters/public-conan)
conan_add_remote(NAME mpusz INDEX 2
        URL https://api.bintray.com/conan/mpusz/conan-mpusz)
conan_cmake_run(CONANFILE conanfile.txt
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

add_compile_options(-Wall -Werror -Wextra -march=native -g -funsafe-math-optimizations)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LTOAvailable)
endif ()

if (LTOAvailable)
    message("Link-time optimization enabled")

    add_link_options(-Wl,--no-as-needed) # Magic found here: https://gcc.gnu.org/ml/gcc-help/2017-03/msg00081.html
else ()
    message("Link-time optimization disabled")
endif ()


add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(benchmarks)

if (LTOAvailable)
    # Clang doesn't support a mix of LTO and non-LTO, so specify everything here
    # Probably a better way to do this but GLOBAL setting didn't work.
    set_property(TARGET oo PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET fp PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET dod PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET util PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET math PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET pt_three_ways PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET raw_to_png PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET oo_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET fp_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET dod_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET util_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET math_tests PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif ()
