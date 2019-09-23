cppcon_add_test(cancelable_no_bases
                SOURCES async_write_with_timeout.cpp
                        main.cpp
                        sticky_cancel_async_write_stream.cpp
                LIBRARIES Boost::boost
                          Boost::system
                          Catch2::Catch2
                          Threads::Threads)
target_link_libraries(cancelable_no_bases_tests
                      testing)
