#include <logstat/thread_pool.hpp>

namespace logstat {

ThreadPool::ThreadPool(unsigned int numThreads)
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

ThreadPool::~ThreadPool()
{
    tasks_.Close(true);
    for (auto & th : threads_) {
        if (th.joinable()) {
            th.join();
        }
    }
}

void ThreadPool::Run()
{
    while (true) {
        Task task;
        if (!tasks_.Get(task)) {
            return;
        }
        try {
            task();
        } catch (...) {
        }
    }
}

} // namespace logstat
