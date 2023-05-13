#pragma once
#include <vector>
#include <atomic>
#include <thread>
#include <math.h>
#include <mutex>

std::mutex mtx;

template <class RandomAccessIterator, class T, class Func>
void CheckForThread(RandomAccessIterator begin, RandomAccessIterator end, std::vector<T>& result,
                    const T& initial, Func func, int step, int index) {

    T cur_value;
    {
        std::scoped_lock lock{mtx};
        cur_value = initial;
    }
    for (auto i = begin; i < end; i += step) {
        cur_value = func(cur_value, *i);
        if (i + step >= end) {
            break;
        }
    }
    {
        std::scoped_lock lock{mtx};
        result[index] = cur_value;
    }
}

template <class RandomAccessIterator, class T, class Func>
T Reduce(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value, Func func) {
    std::vector<T> result;
    std::vector<std::thread> threads;

    int64_t step = std::thread::hardware_concurrency();

    auto size = last - first;
    RandomAccessIterator end = last;
    if (size > step) {
        end = first + step;
    }
    result.resize(std::min(size, step), 0);
    int counter = 0;
    for (auto i = first; i < end; ++i) {
        threads.emplace_back(CheckForThread<RandomAccessIterator, T, Func>, i, last,
                             std::ref(result), initial_value, func, step, counter);
        ++counter;
    }
    for (std::thread& t : threads) {
        t.join();
    }
    auto answer = initial_value;
    for (size_t j = 0; j < result.size(); ++j) {
        answer = func(answer, result[j]);
    }
    return answer;
}
