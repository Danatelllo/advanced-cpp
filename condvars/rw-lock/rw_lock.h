#pragma once
#include <mutex>
#include <vector>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <unordered_set>

class RWLock {
public:
    template <class Func>
    void Read(Func func) {
        {
            std::scoped_lock<std::mutex> lock{mtx_};
            ++is_reading_;
        }
        try {
            func();
        } catch (...) {
            cv_.notify_one();
            throw;
        }
        --is_reading_;
        if (is_reading_ == 0) {
            cv_.notify_all();
        }
    }

    template <class Func>
    void Write(Func func) {
        std::unique_lock<std::mutex> lock{mtx_};
        cv_.wait(lock, [this]() { return is_reading_ == 0; });
        func();
    }

private:
    std::mutex mtx_;
    std::atomic<int> is_reading_ = 0;
    std::condition_variable cv_;
};