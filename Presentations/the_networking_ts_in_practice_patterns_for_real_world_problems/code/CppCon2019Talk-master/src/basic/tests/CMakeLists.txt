cppcon_add_test(basic
                SOURCES allocate_state.cpp
                        async_write_with_timeout.cpp
                        main.cpp
                        op_base.cpp
                        state_base.cpp
                LIBRARIES Boost::boost
                          Boost::system
                          Catch2::Catch2
                          Threads::Threads)
target_link_libraries(basic_tests
                      testing)
