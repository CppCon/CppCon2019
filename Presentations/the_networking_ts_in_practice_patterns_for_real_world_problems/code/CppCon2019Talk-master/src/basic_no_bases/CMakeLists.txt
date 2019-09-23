cppcon_add_library(basic_no_bases SOURCES async_write_with_timeout.cpp
                                  LIBRARIES Boost::boost
                                            Boost::system)
add_subdirectory(tests)
