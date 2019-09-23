add_executable(benchmarks benchmarks.cpp Vec3Benchmarks.cpp)
target_link_libraries(benchmarks math util oo fp dod Threads::Threads CONAN_PKG::google-benchmark)
