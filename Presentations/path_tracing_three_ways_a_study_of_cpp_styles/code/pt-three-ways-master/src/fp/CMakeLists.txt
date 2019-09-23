add_library(fp Render.cpp Triangle.h Triangle.cpp Sphere.cpp Sphere.h Scene.cpp Scene.h Primitive.h SceneBuilder.cpp SceneBuilder.h Render.h optional.hpp)
target_link_libraries(fp math CONAN_PKG::range-v3)
target_include_directories(fp INTERFACE ..)