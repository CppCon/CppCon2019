cppcon_add_library(accept SOURCES async_accept.cpp
                          LIBRARIES Boost::boost
                                    Boost::system)
add_subdirectory(examples)
add_subdirectory(tests)
