// jthreadJoinable.cpp

#include <iostream>
#include "jthread.hpp"

int main(){
    
    std::cout << std::endl;
    std::cout << std::boolalpha;
    
    std::jthread thr{[]{ std::cout << "Joinable std::thread" << std::endl; }};
    
    std::cout << "thr.joinable(): " << thr.joinable() << std::endl;
    
    std::cout << std::endl;
    
}
