add_executable(oo_tests oo_tests.cpp TriangleTests.cpp SphereTests.cpp RendererTests.cpp)
target_link_libraries(oo_tests oo CONAN_PKG::Catch2 Threads::Threads)
add_test(NAME oo_tests COMMAND $<TARGET_FILE:oo_tests>)