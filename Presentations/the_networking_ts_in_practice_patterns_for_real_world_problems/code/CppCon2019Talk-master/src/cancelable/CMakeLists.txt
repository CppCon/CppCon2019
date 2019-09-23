cppcon_add_library(cancelable SOURCES async_write_stream_wrapper.cpp
                                      async_write_with_timeout.cpp
                              LIBRARIES basic
                                        Boost::boost
                                        Boost::system)
add_subdirectory(tests)
