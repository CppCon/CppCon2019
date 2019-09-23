project(include CXX)
set(CMAKE_CXX_STANDARD 11)
add_library(${PROJECT_NAME} INTERFACE)
# Add the parent of this directory, to force code to say
#   #include "include/...." so that it is clear that we
# are using non-standard code... (like features not yet
# included in Approval Tests)
target_include_directories(${PROJECT_NAME}
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
