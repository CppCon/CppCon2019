cppcon_add_test(basic_no_bases
                SOURCES async_write_with_timeout.cpp
                        main.cpp
                LIBRARIES Boost::boost
                          Boost::system
                          Catch2::Catch2
                          Threads::Threads)
target_link_libraries(basic_no_bases_tests
                      testing)
