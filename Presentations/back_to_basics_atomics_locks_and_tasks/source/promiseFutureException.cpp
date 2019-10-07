#include <exception>
#include <future>
#include <iostream>
#include <thread>
#include <utility>

struct Div{
  void operator()(std::promise<int>&& intPromise, int a, int b){
    try{
      if ( b==0 ) throw std::runtime_error("illegal division by zero");
      intPromise.set_value(a/b);
    }
    catch ( ... ){
      intPromise.set_exception(std::current_exception());
    }
  }
};

int main(){

  std::cout << std::endl;

  // define the promises
  std::promise<int> divPromise;
  std::promise<int> divPromise2;

  // get the futures
  std::future<int> divResult= divPromise.get_future();
  std::future<int> divResult2= divPromise2.get_future();

  // calculate the result in a separat thread
  Div div;
  std::thread divThread(div,std::move(divPromise), 20, 0);
  std::thread divThread2(div, std::move(divPromise2), 20, 1);

  // get the result
  try{
    std::cout << "20/0= " << divResult.get() << std::endl;
  }
  catch (std::runtime_error& e){
    std::cout << e.what() << std::endl;
  }
  
  try{
    std::cout << "20/1= " << divResult2.get() << std::endl;
  }
  catch (std::runtime_error& e){
    std::cout << e.what() << std::endl;
  }

  divThread.join();
  divThread2.join();

  std::cout << std::endl;

}

