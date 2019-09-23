cppcon_add_library(work SOURCES async_write_with_timeout.cpp
                                state_base.cpp
                        LIBRARIES basic
                                  Boost::boost
                                  Boost::system
                                  cancelable)
add_subdirectory(tests)
