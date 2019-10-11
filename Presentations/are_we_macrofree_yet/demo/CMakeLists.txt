cmake_minimum_required(VERSION 3.15)
project(macrofree_demo LANGUAGES CXX)

include(CTest)
include(CheckSymbolExists)
INCLUDE(CheckLibraryExists)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

if(BUILD_SHARED_LIBS)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/W3>)
else()
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wall>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wconversion>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wsign-conversion>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wsign-compare>)
endif()

if(MINGW)
    add_definitions($<$<COMPILE_LANGUAGE:CXX>:-DMINGW_HAS_SECURE_API>)
endif()

set(macrofree_demo_srcs
    include/macrofree_demo/c_file_funcs.h
    include/macrofree_demo/sha256.h
    include/macrofree_demo/sha256_openssl.h
    include/macrofree_demo/sha256_implementations.h
    include/macrofree_demo/hexlify.h)

add_library(macrofree_demo)

target_sources(macrofree_demo PRIVATE ${macrofree_demo_srcs})
target_include_directories(macrofree_demo PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include)

if(MSVC OR MINGW)
    target_sources(macrofree_demo PRIVATE src/win32.cc)
else()
    target_sources(macrofree_demo PRIVATE src/posix.cc)
endif()

check_symbol_exists(fopen_s stdio.h FOPEN_S_FOUND)
if(FOPEN_S_FOUND)
    set(HAVE_ANNEX_K true)
else()
    set(HAVE_ANNEX_K false)
endif()

# Cryptography API: Next Generation
if(WIN32)
    target_sources(macrofree_demo PRIVATE src/sha256_cng.cc)
    target_link_libraries(macrofree_demo PRIVATE Bcrypt)
    set(HAVE_CNG true)
else()
    set(HAVE_CNG false)
endif()

# OpenSSL
find_package(OpenSSL)
if(OPENSSL_FOUND)
    target_sources(macrofree_demo PRIVATE src/sha256_openssl.cc)
    target_link_libraries(macrofree_demo PRIVATE OpenSSL::Crypto)
    set(HAVE_CRYPTO true)
else()
    set(HAVE_CRYPTO false)
endif()

configure_file(
    include/macrofree_demo/build_config.h.in
    include/macrofree_demo/build_config.h @ONLY)

set(tests_srcs
    tests/test_c_file_funcs.cc
    tests/test_sha256.cc
    tests/run.cc)
set_source_files_properties(tests/run.cc
    PROPERTIES
    COMPILE_DEFINITIONS DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)

if(BUILD_TESTING)
    find_package(doctest CONFIG REQUIRED)
    add_executable(run ${tests_srcs})
    target_link_libraries(run macrofree_demo doctest::doctest)
    set_property(TARGET run PROPERTY RUNTIME_OUTPUT_DIRECTORY tests)
    add_test(NAME testall COMMAND run WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
