// sharedCounterLocal.cpp

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

int main(){
    
  std::cout << std::endl;
  
  auto constexpr num = 1000;

  std::vector<std::thread> vecThread(num);

  std::atomic<int> i{};

  auto start = std::chrono::steady_clock::now();
  
  for (auto& t: vecThread){
     t = std::thread([&i, &num]{ 
       int local{};
       for(int n = 0; n < num; ++n){ 
         ++local;
       } 
       i += local;
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
                        
    
