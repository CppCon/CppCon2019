if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_SIMULATE_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4
            /WX
            )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4
            /WX
            )
endif ()
