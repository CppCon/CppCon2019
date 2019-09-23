cppcon_add_test(cancelable
                SOURCES async_write_stream_wrapper.cpp
                        async_write_with_timeout.cpp
                        main.cpp
                LIBRARIES Boost::boost
                          Boost::system
                          Catch2::Catch2
                          Threads::Threads)
target_link_libraries(cancelable_tests
                      testing)
