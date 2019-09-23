cppcon_add_test(accept_no_work
                SOURCES async_accept.cpp
                        main.cpp
                LIBRARIES Boost::boost
                          Boost::system
                          Catch2::Catch2
                          Threads::Threads
                          testing)
