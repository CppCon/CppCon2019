cppcon_add_test(accept_one
                SOURCES async_accept.cpp
                        main.cpp
                LIBRARIES Boost::boost
                          Boost::system
                          Catch2::Catch2
                          Threads::Threads)
target_link_libraries(accept_one_tests
                      testing)
