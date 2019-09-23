cppcon_add_library(testing SOURCES allocator.cpp
                                   async_acceptor.cpp
                                   async_write_stream.cpp
                                   clock.cpp
                                   pending_service.cpp
                                   waitable_timer.cpp
                           LIBRARIES Boost::boost
                                     Boost::system)
add_subdirectory(tests)
