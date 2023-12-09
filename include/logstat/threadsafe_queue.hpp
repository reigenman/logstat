#pragma once
#include <mutex>
#include <deque>
#include <condition_variable>

namespace logstat {

template<typename T>
class Queue {
public:
    Queue() = default;
    Queue(const Queue & rhs) = delete;
    Queue & operator=(const Queue & rhs) = delete;

    Queue(Queue && rhs) = default;
    Queue & operator=(Queue && rhs) = default;

    bool Push(T && value)
    {
        {
            std::lock_guard lock(mutex_);
            if (closed_)
                return false;
            deq_.push_back(std::move(value));
        }
        rdCondVar_.notify_one();
    }

    bool Get(T & value)
    {
        std::unique_lock lock(mutex_);
        rdCondVar_.wait(lock, [this] {
            return closed_ || !deq_.empty();
        });
        if (deq_.empty())
            return false;
        value = std::move(deq_.front());
        deq_.pop_front();
        return true;
    }

    void Close(bool clear = true)
    {
        {
            std::lock_guard lock(mutex_);
            closed_ = true;
            if (clear) {
                deq_.clear();
            }
        }
        rdCondVar_.notify_all();
    }
private:
    std::mutex mutex_;
    std::condition_variable rdCondVar_;
    bool closed_{false};
    std::deque<T> deq_;
};

} // namespace logstat
