// sharedCounterAtomicInt.cpp

#include <iostream>
#include <thread>
#include <vector>

int main(){
    
  std::cout << std::endl;
  
  auto constexpr num = 1000;

  std::vector<std::thread> vecThread(num);

  int i{};
  
  for (auto& t: vecThread){
    t = std::thread([&i, &num]{ for(int n = 0; n < num; ++n) std::cout << ++i << " "; });
  }
  
  for (auto& t: vecThread){
    t.join();
  }
  
  std::cout << "\n\n";
  
  std::cout << "num * num:  " << num * num << std::endl;
  std::cout << "i:          " << i << std::endl;
  
  std::cout << "num * num - i: " << num * num - i << std::endl;
  
  std::cout << std::endl;
  
}
                        
    
