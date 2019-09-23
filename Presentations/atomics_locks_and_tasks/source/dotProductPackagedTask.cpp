#include <numeric>
#include <utility>
#include <future>
#include <iostream>
#include <random>
#include <thread>
#include <deque>

static const int NUM = 100000000;

struct SumUp{
  int operator()(std::vector<int>& v, std::vector<int>& w, 
                 int beg, int end){
    return std::inner_product(&v[beg], &v[end], &w[beg], 0LL);
  }
};

int main(){
    
  using workPackage = int(std::vector<int>&, std::vector<int>&, int, int);

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

  SumUp sumUp1;
  SumUp sumUp2;
  SumUp sumUp3;
  SumUp sumUp4;

  // define the tasks
  std::packaged_task<workPackage> sumTask1(sumUp1);
  std::packaged_task<workPackage> sumTask2(sumUp2);
  std::packaged_task<workPackage> sumTask3(sumUp3);
  std::packaged_task<workPackage> sumTask4(sumUp4);

  // get the futures
  std::future<int> sumResult1 = sumTask1.get_future();
  std::future<int> sumResult2 = sumTask2.get_future();
  std::future<int> sumResult3 = sumTask3.get_future();
  std::future<int> sumResult4 = sumTask4.get_future();

  // push the tasks on the container
  std::deque<std::packaged_task<workPackage>> allTasks;
  allTasks.push_back(std::move(sumTask1));
  allTasks.push_back(std::move(sumTask2));
  allTasks.push_back(std::move(sumTask3));
  allTasks.push_back(std::move(sumTask4));
  
  int begin{1};
  int increment{NUM / 4};
  int end= begin + increment;
  
  // measure the execution time
  std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
  
  // execute each task in a separate thread
  while (! allTasks.empty()){
    std::packaged_task<workPackage> myTask= std::move(allTasks.front());
    allTasks.pop_front();
    std::thread sumThread(std::move(myTask), v, w, begin, end);
    begin = end;
    end += increment;
    sumThread.detach();
  }

  // get the results
  auto sum = sumResult1.get() + sumResult2.get() + sumResult3.get() + sumResult4.get();
  
  std::cout << "getDotProduct(v, w): " << sum << std::endl;
  std::chrono::duration<double> dur  = std::chrono::system_clock::now() - start;
  std::cout << "Parallel Execution: "<< dur.count() << std::endl;

  std::cout << std::endl;

}
