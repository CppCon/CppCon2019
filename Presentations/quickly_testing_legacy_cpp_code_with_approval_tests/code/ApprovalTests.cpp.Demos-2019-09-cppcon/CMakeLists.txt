cmake_minimum_required(VERSION 3.8)

# disable testsuite if used via add_subdirectory()
if (NOT DEFINED PROJECT_NAME)
    set(NOT_SUBPROJECT ON)
endif ()

# Provide path for scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

project(ApprovalTests.cpp)
#add_subdirectory(ApprovalTests)

set(CMAKE_VERBOSE_MAKEFILE off)

if (NOT_SUBPROJECT)
    # Download and unpack googletest at configure time
    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build .
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")

    # Prevent GoogleTest from overriding our compiler/linker options
    # when building with Visual Studio
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This adds
    # the following targets: gtest, gtest_main, gmock
    # and gmock_main
    add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
            "${CMAKE_BINARY_DIR}/googletest-build")

    enable_testing()
    add_subdirectory(third_party)
    add_subdirectory(include)

    add_subdirectory(catch1_demo)
    add_subdirectory(catch2_demo)
    add_subdirectory(doctest_demo)
    add_subdirectory(google_test_demo)
endif ()
