cppcon_add_library(accept_all SOURCES async_accept.cpp
                              LIBRARIES accept_one
                                        Boost::boost
                                        Boost::system)
add_subdirectory(tests)
