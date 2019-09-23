cppcon_add_library(accept_one SOURCES async_accept.cpp
                              LIBRARIES Boost::boost
                                        Boost::system)
add_subdirectory(tests)
