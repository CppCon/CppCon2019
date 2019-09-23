add_executable(math_tests math_tests.cpp Vec3Tests.cpp Norm3Tests.cpp RayTests.cpp OrthoNormalBasisTests.cpp)
target_link_libraries(math_tests math CONAN_PKG::Catch2)
add_test(NAME math_tests COMMAND $<TARGET_FILE:math_tests>)