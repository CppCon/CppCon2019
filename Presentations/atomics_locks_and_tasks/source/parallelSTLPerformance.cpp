// parallelSTLPerformance.cpp

#include <algorithm>
#include <cmath>
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <string>
#include <vector>

constexpr long long size = 500'000'000;  
  
const double pi = std::acos(-1);

template <typename Func>
void getExecutionTime(const std::string& title, Func func){
    
     const auto start = std::chrono::steady_clock::now();
     func();
     const std::chrono::duration<double> dur = std::chrono::steady_clock::now() - start;
     std::cout << title << ": " << dur.count() << " sec. " << std::endl;
     
}
    
int main(){

    std::cout << std::endl;
    
    std::vector<double> randValues;
    randValues.reserve(size);
   
    std::mt19937 engine;
    std::uniform_real_distribution<> uniformDist(0, pi / 2);
    for (long long i = 0 ; i < size ; ++i) randValues.push_back(uniformDist(engine));
    
    std::vector<double> workVec(randValues);
    
    getExecutionTime("std::execution::seq", [workVec]() mutable {
        std::transform(std::execution::seq, workVec.begin(), workVec.end(), 
		                   workVec.begin(), 
                           [](double arg){ return std::tan(arg); }
                       );
    });
        
     getExecutionTime("std::execution::par", [workVec]() mutable {
        std::transform(std::execution::par, workVec.begin(), workVec.end(), 
		                   workVec.begin(), 
                           [](double arg){ return std::tan(arg); }
                       );
    });
     
    getExecutionTime("std::execution::par_unseq", [workVec]() mutable {
        std::transform(std::execution::par_unseq, workVec.begin(), workVec.end(), 
		                   workVec.begin(), 
                           [](double arg){ return std::tan(arg); }
                      );
    });
    
}
