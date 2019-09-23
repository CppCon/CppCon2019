add_executable(accept_example accept_example.cpp)
target_link_libraries(accept_example
                      accept
                      Boost::boost
                      Boost::system
                      Threads::Threads)
