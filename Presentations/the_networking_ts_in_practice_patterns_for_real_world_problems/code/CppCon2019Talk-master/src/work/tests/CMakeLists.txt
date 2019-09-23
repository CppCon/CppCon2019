cppcon_add_test(work
                SOURCES async_write_with_timeout.cpp
                        main.cpp
                        state_base.cpp
                LIBRARIES Boost::boost
                          Boost::system
                          Catch2::Catch2
                          Threads::Threads)
target_link_libraries(work_tests
                      testing)
