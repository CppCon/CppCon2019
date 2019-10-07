// executor.h                                                         -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2019 Dietmar Kuehl http://www.dietmar-kuehl.de         
//                                                                       
//  Permission is hereby granted, free of charge, to any person          
//  obtaining a copy of this software and associated documentation       
//  files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify,        
//  merge, publish, distribute, sublicense, and/or sell copies of        
//  the Software, and to permit persons to whom the Software is          
//  furnished to do so, subject to the following conditions:             
//                                                                       
//  The above copyright notice and this permission notice shall be       
//  included in all copies or substantial portions of the Software.      
//                                                                       
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        
//  OTHER DEALINGS IN THE SOFTWARE. 
// ----------------------------------------------------------------------------

#ifndef INCLUDED_EXECUTOR
#define INCLUDED_EXECUTOR

#include "function.h"
#ifdef USE_TBB
#include <tbb/task_group.h>
#endif
#include <atomic>
#include <deque>
#include <list>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <utility>

// ----------------------------------------------------------------------------

namespace nstd {
    class immediate_executor;
    class pool_executor;
    class multipool_executor;
    class job_stealing_executor;
    class tbb_executor;
}

// ----------------------------------------------------------------------------

class nstd::immediate_executor {
public:
    explicit  immediate_executor(int =  0) {}
    template <typename Fun>
    void execute(Fun&& fun) {
        fun();
    }
};

// ----------------------------------------------------------------------------

struct jthread
    : std::thread {
    using std::thread::thread;
    ~jthread() { this->std::thread::join(); }
    void join() = delete;
    void detach() = delete;
};

class thread_pool {
private:
    bool                               d_flag{true};
    std::mutex                         d_mutex;
    std::condition_variable            d_condition;
    std::deque<nstd::function<void()>> d_queue;
    std::deque<jthread>                d_pool;

    void work() {
        while (true) {
            std::unique_lock kerberos(this->d_mutex);
            this->d_condition.wait(kerberos,
                                   [this]{ return !this->d_queue.empty() || !this->d_flag; });
            if (this->d_queue.empty()) {
                break;
            }
            auto fun(std::move(this->d_queue.front()));
            this->d_queue.pop_front();
            kerberos.unlock();
            fun();
        }
    }
public:
    thread_pool(int count) {
        std::lock_guard kerberos(this->d_mutex);
        count = std::max(1, count);
        for (int i{0}; i != count; ++i) { 
            this->d_pool.emplace_back(&thread_pool::work, this);
        }
    }
    thread_pool(thread_pool&&) = delete;
    ~thread_pool() { this->stop(); }
    void stop() {
        std::lock_guard kerberos(this->d_mutex);
        this->d_flag = false;
        this->d_condition.notify_all();
    }
    template <typename Fun>
    void execute(Fun&& fun) {
        {
            std::lock_guard kerberos(this->d_mutex);
            this->d_queue.emplace_back(std::forward<Fun>(fun));
        }
        this->d_condition.notify_one();
    }
};

class nstd::pool_executor {
private:
    std::shared_ptr<thread_pool> d_pool;
public:
    explicit pool_executor(int count = std::thread::hardware_concurrency())
        : d_pool(std::make_shared<thread_pool>(count)) {
    }
    void stop() { this->d_pool->stop(); }
    template <typename Fun>
    void execute(Fun&& fun) { this->d_pool->execute(std::forward<Fun>(fun)); }
};

class nstd::multipool_executor {
private:
    unsigned int                              d_next = 0ul;
    unsigned int const                        d_count;
    std::vector<std::shared_ptr<thread_pool>> d_pools;
public:
    explicit multipool_executor(int count = std::thread::hardware_concurrency())
        : d_count(std::max(2, count / 16))
        , d_pools(std::thread::hardware_concurrency() / count) {
        for (auto& pool: this->d_pools) {
            pool = std::make_shared<thread_pool>(std::thread::hardware_concurrency() / d_count);
        }
    }
    template <typename Fun>
    void execute(Fun&& fun) { this->d_pools[this->d_next % this->d_count]->execute(std::forward<Fun>(fun)); }
};

// ----------------------------------------------------------------------------

class nstd::job_stealing_executor {
private:
    struct worker {
        nstd::function<nstd::function<void()>()> d_thief;
        std::atomic<bool>                        d_continue;
        std::mutex                               d_mutex;
        std::deque<nstd::function<void()>>       d_queue;
        std::thread                              d_thread;

        explicit worker(nstd::function<nstd::function<void()>()>&& thief)
            : d_thief(std::move(thief))
            , d_continue(true)
            , d_mutex()
            , d_queue()
            , d_thread() {
        }
        ~worker() {
            this->d_continue = false;
            this->d_thread.join();
        }

        void start() {
            this->d_thread = std::thread(&worker::work, this);
        }
        void stop() {
            this->d_continue = false;
        }
        void work() {
            while (this->d_continue) {
                std::unique_lock kerberos(this->d_mutex);
                if (!this->d_queue.empty()) {
                    auto job(std::move(this->d_queue.back()));
                    this->d_queue.pop_back();
                    kerberos.unlock();
                    job();
                    continue;
                }
                kerberos.unlock();
                if (auto job = this->d_thief()) {
                    job();
                }
            }
        }
        template <typename Fun>
        void execute(Fun&& fun) {
            std::lock_guard kerberos(this->d_mutex);
            this->d_queue.emplace_back(std::forward<Fun>(fun));
        }
        nstd::function<void()> steal() {
            std::lock_guard kerberos(this->d_mutex);
            if (this->d_queue.empty()) {
                return nstd::function<void()>();
            }
            nstd::function<void()> rc(std::move(this->d_queue.front()));
            this->d_queue.pop_front();
            return rc;
        }
    };
    struct pool {
        std::deque<worker>        d_worker;
        std::atomic<unsigned int> d_next;
        explicit pool(unsigned int count) {
            for (unsigned int i(0u); i != count; ++i) {
                this->d_worker.emplace_back(
                   [count, this, next = 0u, stride = 3u]() mutable {
                       next =  (next + stride) % count;
                       return this->d_worker[next].steal();
                   });
            }
            for (auto& worker: this->d_worker) {
                worker.start();
            }
        }
        template <typename Fun>
        void execute(Fun&& fun) {
            auto& worker = this->d_worker[++this->d_next % this->d_worker.size()];
            worker.execute(std::forward<Fun>(fun));
        }
    };
    std::shared_ptr<pool> d_pool;
    
public:
    explicit job_stealing_executor(int count = std::thread::hardware_concurrency())
        : d_pool(std::make_shared<pool>(count)) {
    }
    template <typename Fun>
    void execute(Fun&& fun) {
        this->d_pool->execute(std::forward<Fun>(fun));
    }
};

// ----------------------------------------------------------------------------

#ifdef USE_TBB
class nstd::tbb_executor {
private:
    struct pool {
        tbb::task_group d_group;
        ~pool() { this->d_group.wait(); }
        template <typename Fun>
        void execute(Fun&& fun) {
            this->d_group.run(std::forward<Fun>(fun));
        }
    };
    std::shared_ptr<pool> d_pool;
public:
    explicit tbb_executor(int = 0): d_pool(std::make_shared<pool>()) {}
    template <typename Fun>
    void execute(Fun&& fun) {
        this->d_pool->execute(std::forward<Fun>(fun));
    }
};
#endif

// ----------------------------------------------------------------------------

#endif
