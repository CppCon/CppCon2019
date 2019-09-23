// threadSafeInterface.cpp

#include <iostream>
#include <mutex>
#include <thread>

class Critical{

public:
    void interface1() const {
        std::lock_guard<std::mutex> lockGuard(mut);
        implementation1();
    }
  
    void interface2(){
        std::lock_guard<std::mutex> lockGuard(mut);
        implementation2();
        implementation3();
        implementation1();
    }
   
private: 
    void implementation1() const {
        std::cout << "implementation1: " 
                  << std::this_thread::get_id() << std::endl;
    }
    void implementation2(){
        std::cout << "    implementation2: " 
                  << std::this_thread::get_id() << std::endl;
    }
    void implementation3(){    
        std::cout << "        implementation3: " 
                  << std::this_thread::get_id() << std::endl;
    }
  

mutable std::mutex mut;

};

int main(){
    
    std::cout << std::endl;
    
    std::thread t1([]{ 
        const Critical crit;
        crit.interface1();
    });
    
    std::thread t2([]{
        Critical crit;
        crit.interface2();
        crit.interface1();
    });
    
    Critical crit;
    crit.interface1();
    crit.interface2();
    
    t1.join();
    t2.join();    
    
    std::cout << std::endl;
    
}
