cppcon_add_library(cancelable_no_bases SOURCES async_write_with_timeout.cpp
                                               sticky_cancel_async_write_stream.cpp
                                       LIBRARIES Boost::boost
                                                 Boost::system)
add_subdirectory(tests)
