#include <future>
#include <iostream>
#include <utility>


void doTheWork(){
  std::cout << "Processing shared data." << std::endl;
}

void waitingForWork(std::future<void>&& fut){

    std::cout << "Worker: Waiting for work." << std::endl;
    fut.wait();
    doTheWork();
    std::cout << "Work done." << std::endl;

}

void setDataReady(std::promise<void>&& prom){

    std::cout << "Sender: Data is ready."  << std::endl;
    prom.set_value();

}

int main(){

  std::cout << std::endl;

  std::promise<void> sendReady;
  auto fut= sendReady.get_future();

  std::thread t1(waitingForWork, std::move(fut));
  std::thread t2(setDataReady, std::move(sendReady));

  t1.join();
  t2.join();

  std::cout << std::endl;
  
}

