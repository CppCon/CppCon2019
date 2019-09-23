add_subdirectory(math)
add_subdirectory(oo)
add_subdirectory(fp)
add_subdirectory(dod)
add_subdirectory(util)

add_test(NAME seed_tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND bash -c "${CMAKE_CURRENT_SOURCE_DIR}/seed_tests.sh $<TARGET_FILE:pt_three_ways> ${CMAKE_CURRENT_BINARY_DIR}")
