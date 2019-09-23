add_executable(fp_tests fp_tests.cpp TriangleTests.cpp SphereTests.cpp)
target_link_libraries(fp_tests fp CONAN_PKG::Catch2)
add_test(NAME fp_tests COMMAND $<TARGET_FILE:fp_tests>)