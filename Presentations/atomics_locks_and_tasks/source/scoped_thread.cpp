// scoped_thread.cpp

#include <iostream>
#include <thread>
#include <utility>


class scoped_thread{
  std::thread t;
public:
  explicit scoped_thread(std::thread t_): t(std::move(t_)){
    if ( !t.joinable() ) throw std::logic_error("No thread");
  }
  ~scoped_thread(){
    t.join();
  }
  scoped_thread(scoped_thread&)= delete;
  scoped_thread& operator=(scoped_thread const &)= delete;
};

int main(){

  scoped_thread t(std::thread([]{std::cout << std::this_thread::get_id() << std::endl;}));

}