add_executable(util_tests util_tests.cpp ObjLoaderTests.cpp ArrayOutputTests.cpp)
target_link_libraries(util_tests util CONAN_PKG::Catch2)
add_test(NAME util_tests COMMAND $<TARGET_FILE:util_tests>)