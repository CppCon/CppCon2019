add_library(oo Primitive.cpp Primitive.h Scene.cpp Scene.h Triangle.cpp Triangle.h Sphere.cpp Sphere.h Renderer.cpp Renderer.h SceneBuilder.cpp SceneBuilder.h Material.cpp Material.h)
target_link_libraries(oo math util)
target_include_directories(oo INTERFACE ..)
