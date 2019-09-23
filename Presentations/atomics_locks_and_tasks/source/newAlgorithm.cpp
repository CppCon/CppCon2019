// newAlgorithm.cpp

#include <algorithm>
#include <execution>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>


int main(){
  
  std::cout << std::endl;
  
  // for_each_n
  
  std::vector<int> intVec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};                       
  std::for_each_n(std::execution::par,                      
                  intVec.begin(), 5, [](int& arg){ arg *= arg; });
  
  std::cout << "for_each_n: ";
  for (auto v: intVec) std::cout << v << " ";
  std::cout << "\n\n";
  
  // exclusive_scan and inclusive_scan
  std::vector<int> resVec{1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::exclusive_scan(std::execution::par,                   
                      resVec.begin(), resVec.end(), resVec.begin(), 1,
                      [](int fir, int sec){ return fir * sec; });
  
  std::cout << "exclusive_scan: ";
  for (auto v: resVec) std::cout << v << " ";
  std::cout << std::endl; 
  
  std::vector<int> resVec2{1, 2, 3, 4, 5, 6, 7, 8, 9};
  
  std::inclusive_scan(std::execution::par,                                      
                      resVec2.begin(), resVec2.end(), resVec2.begin(),        
                      [](int fir, int sec){ return fir * sec; }, 1);
  
  std::cout << "inclusive_scan: ";
  for (auto v: resVec2) std::cout << v << " ";
  std::cout << "\n\n";
  
  // transform_exclusive_scan and transform_inclusive_scan
  std::vector<int> resVec3{1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<int> resVec4(resVec3.size());             
  std::transform_exclusive_scan(std::execution::par,         
                                resVec3.begin(), resVec3.end(), 
                                resVec4.begin(), 0,
                                [](int fir, int sec){ return fir + sec; },
                                [](int arg){ return arg *= arg; });
  
  std::cout << "transform_exclusive_scan: ";
  for (auto v: resVec4) std::cout << v << " ";
  std::cout << std::endl;
  
  std::vector<std::string> strVec{"Only","for","testing","purpose"};             
  std::vector<int> resVec5(strVec.size());
  
  std::transform_inclusive_scan(std::execution::par,        
                                strVec.begin(), strVec.end(), 
                                resVec5.begin(), 0,
                                [](auto fir, auto sec){ return fir + sec; },
                                [](auto s){ return s.length(); });
                                  
  std::cout << "transform_inclusive_scan: ";
  for (auto v: resVec5) std::cout << v << " ";
  std::cout << "\n\n";
  
  // reduce and transform_reduce
  std::vector<std::string> strVec2{"Only","for","testing","purpose"};
  
  std::string res = std::reduce(std::execution::par,        
                    strVec2.begin() + 1, strVec2.end(), strVec2[0], 
                    [](auto fir, auto sec){ return fir + ":" + sec; });
  
  std::cout << "reduce: " << res << std::endl;
  
  std::size_t res7 = std::transform_reduce(std::execution::par,        
                     strVec2.begin(), strVec2.end(), 
                     [](std::string s){ return s.length(); }, 
                     0, [](std::size_t a, std::size_t b){ return a + b; });
                                                                            
    
  std::cout << "transform_reduce: " << res7 << std::endl;
  
  std::cout << std::endl;

}



