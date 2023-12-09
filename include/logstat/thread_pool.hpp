#pragma once

#include <thread>
#include <functional>
#include "threadsafe_queue.hpp"

namespace logstat {

class ThreadPool {
public:
    using Task = std::function<void ()>;
    explicit ThreadPool(unsigned numThreads = 0);
    ~ThreadPool();

    ThreadPool(const ThreadPool & rhs) = delete;
    ThreadPool & operator=(const ThreadPool & rhs) = delete;

    template<typename Func>
    void Post(Func && task)
    {
        tasks_.Push(Task(std::forward<Func>(task)));
    }
private:
    void Run();
    Queue<Task> tasks_;
    std::vector<std::thread> threads_;
};


} // namespace logstat
