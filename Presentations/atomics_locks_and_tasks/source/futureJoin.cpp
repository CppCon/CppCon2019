// futureJoin.cpp

#include <future>
#include <iostream>


int main(){
    
    std::cout << std::endl;
    
    std::future fut = std::async([]{ std::cout << "Joins automatically" << std::endl; });
    
    fut.get();
    
    std::cout << std::endl;
    
}