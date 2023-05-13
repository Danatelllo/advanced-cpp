#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>

class DefaultCallback {
public:
    void operator()(int& value) {
        --value;
    }
};

class Semaphore {
public:
    Semaphore(int count) : count_(count) {
    }

    void Leave() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.pop_front();
        ++count_;
        if (!cv_.empty()) {
            cv_[0]->first.notify_one();
            cv_[0]->second = true;
        }
    }

    template <class Func>
    void Enter(Func callback) {
        std::unique_lock<std::mutex> lock(mutex_);

        std::shared_ptr<std::pair<std::condition_variable, bool>> current;
        current = std::make_shared<std::pair<std::condition_variable, bool>>();
        cv_.push_back(current);

        current->first.wait(lock, [&]() { return !count_ || !current->second; });

        callback(count_);
    }

    void Enter() {
        DefaultCallback callback;
        Enter(callback);
    }

private:
    std::mutex mutex_;
    std::deque<std::shared_ptr<std::pair<std::condition_variable, bool>>> cv_;
    int count_ = 0;
};
