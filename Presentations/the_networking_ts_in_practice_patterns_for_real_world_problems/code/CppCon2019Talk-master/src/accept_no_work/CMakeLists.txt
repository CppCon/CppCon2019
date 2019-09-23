cppcon_add_library(accept_no_work SOURCES async_accept.cpp
                                  LIBRARIES Boost::boost
                                            Boost::system)
add_subdirectory(tests)
