cppcon_add_test(accept
                SOURCES async_accept.cpp
                        main.cpp
                LIBRARIES Boost::boost
                          Boost::system
                          Catch2::Catch2
                          Threads::Threads)
target_link_libraries(accept_tests
                      testing)
