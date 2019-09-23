add_executable(small_pt smallpt.cpp)
target_compile_options(small_pt PUBLIC -fopenmp)
find_package(OpenMP)
target_link_libraries(small_pt PUBLIC OpenMP::OpenMP_CXX)