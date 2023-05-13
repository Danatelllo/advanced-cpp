#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>
#include <set>
#include <mutex>
#include <thread>
#include <iostream>

template <class T>
class TimerQueue {
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

public:
    void Add(const T& item, TimePoint at) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            clocks_.insert(std::pair(at, item));
        }
        cv_.notify_one();
    }

    T Pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]() { return !clocks_.empty(); });
        cv_.wait_until(lock, clocks_.begin()->first);
        auto now = clocks_.begin()->first;
//        lock.unlock();
//        while (now >= Clock::now()) {
//            now = clocks_.begin()->first;
//        }
        return Get();
    }

private:
    T Get() {
        auto answer = clocks_.begin()->second;
        clocks_.erase(*clocks_.begin());
        return answer;
    }
    std::mutex mtx_;
    std::condition_variable cv_;
    std::multiset<std::pair<TimePoint, T>> clocks_;
};
