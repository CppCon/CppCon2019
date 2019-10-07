#include <condition_variable>
#include <functional>
#include <queue>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

template <typename T>
class Monitor{
public:
    void lock() const {
        monitMutex.lock();
    }
    void unlock() const {
        monitMutex.unlock();
    }
    
    void notify_one() const noexcept {
        monitCond.notify_one();
    }
    void wait() const {
        std::unique_lock<std::recursive_mutex> monitLock(monitMutex);
        monitCond.wait(monitLock);
    }
    
private:
    mutable std::recursive_mutex monitMutex;
    mutable std::condition_variable_any monitCond;
};

template <typename T>
class ThreadSafeQueue: public Monitor<ThreadSafeQueue<T>>{
public:
    void add(T val){ 
        derived.lock();
        myQueue.push(val);
        derived.unlock();
        derived.notify_one();
    } 
    
    T get(){ 
        derived.lock();
        while (myQueue.empty()) derived.wait();
        auto val = myQueue.front();
        myQueue.pop();
        derived.unlock();
        return val;
    }
private:
    std::queue<T> myQueue;
    ThreadSafeQueue<T>& derived = static_cast<ThreadSafeQueue<T>&>(*this);
};

class Dice{
public:
    int operator()(){ return rand(); }
private:
    std::function<int()> rand = std::bind(std::uniform_int_distribution<>(1, 6), 
                                          std::default_random_engine());
};


int main(){
    
    std::cout << std::endl;
    
    constexpr auto NUM = 100;
    
    ThreadSafeQueue<int> safeQueue;
    auto addLambda = [&safeQueue](int val){ safeQueue.add(val); };
    auto getLambda = [&safeQueue]{ std::cout << safeQueue.get() << " " 
                                             << std::this_thread::get_id() << "; "; };
        
    std::vector<std::thread> addThreads(NUM);
    Dice dice;
    for (auto& thr: addThreads) thr = std::thread(addLambda, dice() );
    
    std::vector<std::thread> getThreads(NUM);
    for (auto& thr: getThreads) thr = std::thread(getLambda);

    for (auto& thr: addThreads) thr.join();
    for (auto& thr: getThreads) thr.join();
    
    std::cout << "\n\n";
     
}
