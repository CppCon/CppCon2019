add_executable(pt_three_ways main.cpp PngWriter.cpp PngWriter.h)
target_link_libraries(pt_three_ways math oo fp dod util Threads::Threads CONAN_PKG::clara CONAN_PKG::libpng)

add_executable(raw_to_png raw_to_png.cpp PngWriter.cpp PngWriter.h)
target_link_libraries(raw_to_png math util Threads::Threads CONAN_PKG::clara CONAN_PKG::libpng)
