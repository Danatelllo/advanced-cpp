#pragma once

#include <queue>
#include <atomic>
#include <iostream>
#include <optional>

template <class T>
struct Element {
    Element(){};
    Element(int j, T value) : generation(j){};

    std::optional<T> mean;
    std::atomic<int> generation;
};

template <class T>
class MPMCBoundedQueue {
public:
    explicit MPMCBoundedQueue(int size) : backet_(size) {
        for (int j = 0; j < size; ++j) {
            backet_[j].generation = j;
        }
        max_size_ = size - 1;
    }

    bool Enqueue(const T& value) {
        int i_current = i_write_;
        while (true) {
            int p_current = i_current & max_size_;
            int g_current = backet_[p_current].generation;
            if (g_current == i_current) {
                if (i_write_.compare_exchange_weak(i_current, i_current + 1)) {
                    backet_[p_current].mean = value;
                    backet_[p_current].generation = g_current + 1;
                    return true;
                }
            } else if (i_current >= g_current) {
                return false;
            } else {
                i_current = i_write_;
            }
        }
    }

    bool Dequeue(T& data) {
        int i_current = i_pop_;
        while (true) {
            int p_current = i_current & max_size_;
            int g_current = backet_[p_current].generation;
            if (g_current == i_current + 1) {
                if (i_pop_.compare_exchange_weak(i_current, i_current + 1)) {
                    data = backet_[p_current].mean.value();
                    backet_[p_current].generation = i_current + backet_.size();
                    return true;
                }
            } else if (i_current >= g_current) {
                return false;
            } else {
                i_current = i_pop_;
            }
        }
    }

private:
    std::atomic<int> i_write_ = 0;
    std::atomic<int> i_pop_ = 0;
    std::vector<Element<T>> backet_;
    int max_size_ = 0;
};
