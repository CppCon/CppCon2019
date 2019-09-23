cppcon_add_library(basic SOURCES allocate_state.cpp
                                 async_write_with_timeout.cpp
                                 op_base.cpp
                                 state_base.cpp
                         LIBRARIES Boost::boost
                                   Boost::system)
add_subdirectory(tests)
