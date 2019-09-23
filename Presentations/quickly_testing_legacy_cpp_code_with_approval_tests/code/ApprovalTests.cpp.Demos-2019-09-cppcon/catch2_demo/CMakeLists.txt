project(catch2_demo)
set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME}
        main.cpp
        DisposableExplanation.cpp
        Test01.cpp
        Test02.cpp
        Test03CustomReporters.cpp
        Test04ConsoleReporter.cpp
        Test05VerifyAll.cpp
        Test06VerifyAllCombinations.cpp
        Test07TestScenarios.cpp
        )
target_link_libraries(${PROJECT_NAME} ApprovalTests include catch2)
target_compile_definitions(${PROJECT_NAME} PRIVATE CATCH_CONFIG_FAST_COMPILE)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

#include(WarningsAsErrors)
