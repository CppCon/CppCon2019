#include <iostream>
#include <thread>

int main(){

  std::thread t{[]{ std::cout << "New thread"; }};      
  std::cout << "t.joinable(): " << t.joinable(); 

} 
