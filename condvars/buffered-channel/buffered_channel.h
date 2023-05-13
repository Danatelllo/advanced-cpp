#pragma once

#include <utility>
#include <optional>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <mutex>

template <class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) {
        size_ = static_cast<size_t>(size);
    }

    void Send(const T& value) {
        std::unique_lock<std::mutex> lock{mtx_};
        if (is_closed_) {
            throw std::runtime_error("tototo");
        }
        s_cv_.wait(lock, [this]() { return data_.size() != size_ || is_closed_; });
        if (is_closed_) {
            throw std::runtime_error("tototo");
        }
        data_.push(value);
        r_cv_.notify_all();
    }

    std::optional<T> Recv() {
        // Your code goes here
        std::unique_lock<std::mutex> lock{mtx_};
        if (is_closed_ && data_.empty()) {
            return std::nullopt;
        }
        r_cv_.wait(lock, [this]() { return !data_.empty() || is_closed_; });
        if (is_closed_ && data_.empty()) {
            return std::nullopt;
        }
        T meaning = data_.front();
        data_.pop();
        s_cv_.notify_all();

        return meaning;
    }

    void Close() {
        std::scoped_lock<std::mutex> lock{mtx_};
        is_closed_ = true;
        s_cv_.notify_all();
        r_cv_.notify_all();
    }

private:
    std::queue<T> data_;
    size_t size_;
    std::condition_variable s_cv_;
    std::mutex mtx_;
    std::condition_variable r_cv_;
    bool is_closed_ = false;
};
