// dotProductThread.cpp

#include <chrono>
#include <iostream>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

static const int NUM = 100000000;

long long getDotProduct(std::vector<int>& v, std::vector<int>& w){
    
  long long res{};
  std::mutex mut;
  
  std::thread t1{[&]{
      std::lock_guard<std::mutex> lockRes(mut);
      auto prod = std::inner_product(&v[0], &v[v.size()/4], &w[0], 0LL);
      res += prod;
    }
  };
  std::thread t2{[&]{
      std::lock_guard<std::mutex> lockRes(mut);
      auto prod = std::inner_product(&v[v.size()/4], &v[v.size()/2], &w[v.size()/4], 0LL);
      res += prod;
    }
  };
  std::thread t3{[&]{
      std::lock_guard<std::mutex> lockRes(mut);
      auto prod = std::inner_product(&v[v.size()/2], &v[v.size()*3/4], &w[v.size()/2], 0LL);
      res += prod;
    }
  };
  std::thread t4{[&]{
      std::lock_guard<std::mutex> lockRes(mut);
      auto prod = std::inner_product(&v[v.size()*3/4], &v[v.size()], &w[v.size()*3/4], 0LL);
      res += prod;
    }
  };
  
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  
  return res;
}


int main(){

  std::cout << std::endl;

  // get NUM random numbers from 0 .. 100
  std::random_device seed;

  // generator
  std::mt19937 engine(seed());

  // distribution
  std::uniform_int_distribution<int> dist(0, 100);

  // fill the vectors
  std::vector<int> v, w;
  v.reserve(NUM);
  w.reserve(NUM);
  for (int i=0; i< NUM; ++i){
    v.push_back(dist(engine));
    w.push_back(dist(engine));
  }

  // measure the execution time
  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
  std::cout << "getDotProduct(v, w): " << getDotProduct(v, w) << std::endl;
  std::chrono::duration<double> dur  = std::chrono::system_clock::now() - start;
  std::cout << "Parallel Execution: "<< dur.count() << std::endl;

  std::cout << std::endl;

}
