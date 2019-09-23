cppcon_add_test(testing
                SOURCES allocator.cpp
                        async_acceptor.cpp
                        async_write_stream.cpp
                        clock.cpp
                        main.cpp
                        pending_service.cpp
                        waitable_timer.cpp
                LIBRARIES Boost::boost
                          Boost::system
                          Catch2::Catch2
                          Threads::Threads)
