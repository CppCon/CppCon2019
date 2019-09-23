add_executable(dod_tests dod_tests.cpp SceneTests.cpp SphereTests.cpp TriangleTests.cpp)
target_link_libraries(dod_tests dod CONAN_PKG::Catch2)
add_test(NAME dod_tests COMMAND $<TARGET_FILE:dod_tests>)