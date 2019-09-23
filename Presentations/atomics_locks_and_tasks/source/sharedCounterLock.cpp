// sharedCounterLock.cpp

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

int main(){
    
  std::cout << std::endl;
  
  auto constexpr num = 1000;

  std::vector<std::thread> vecThread(num);

  int i{};
  
  std::mutex mut;
  
  auto start = std::chrono::steady_clock::now();
  
  for (auto& t: vecThread){
    t = std::thread([&i, &mut, &num]{ for(int n = 0; n < num; ++n){
        std::lock_guard<std::mutex> lo(mut);
        ++i;
      }
    });
  }
  
  std::chrono::duration<double> dur = std::chrono::steady_clock::now() - start;
  
  for (auto& t: vecThread){
    t.join();
  }
  
  std::cout << "i: " << i << std::endl;
  std::cout << dur.count() << " seconds" << std::endl;
  
  std::cout << std::endl;
  
}
                        
    
