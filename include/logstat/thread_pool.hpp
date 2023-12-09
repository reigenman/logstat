#pragma once

#include <thread>
#include <functional>
#include "threadsafe_queue.hpp"

namespace logstat {


class ThreadPool {
public:
    using Task = std::function<void ()>;
    explicit ThreadPool(unsigned numThreads = 0)
    {
        if (numThreads == 0) {
            numThreads = std::thread::hardware_concurrency();
        }
        numThreads = std::max(numThreads, 1u);
        for (unsigned i = 0; i < numThreads; ++i) {
            threads_.emplace_back([this] {
                this->Run();
            });
        }
    }

    ~ThreadPool()
    {
        for (auto & th : threads_) {
            if (th.joinable()) {
                th.join();
            }
        }
    }
private:
    void Run()
    {
    }
    Queue<Task> tasks_;
    std::vector<std::thread> threads_;
};


} // namespace logstat
