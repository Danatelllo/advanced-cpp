#pragma once

#include <atomic>
#include <optional>
#include <stdexcept>
#include <utility>

template <class T>
class MPSCStack {
    struct Node {
        Node(const T v, Node* next) : value(v), from(next) {
        }
        const T value;
        Node* from;
    };
    std::atomic<Node*> head_;

public:
    // Push adds one element to stack top.
    //
    // Safe to call from multiple threads.
    void Push(const T& value) {
        Node* new_head = new Node(value, head_.load());
        while (!head_.compare_exchange_weak(new_head->from, new_head)) {
        };
    }

    // Pop removes top element from the stack.
    //
    // Not safe to call concurrently.
    std::optional<T> Pop() {
        if (head_.load() == nullptr) {
            return std::nullopt;
        }
        Node* now = head_.load();
        while (!head_.compare_exchange_weak(now, now->from)) {
        };
        T answer = now->value;
        delete now;
        return std::optional<T>(answer);
    }

    // DequeuedAll Pop's all elements from the stack and calls cb() for each.
    //
    // Not safe to call concurrently with Pop()
    template <class TFn>
    void DequeueAll(const TFn& cb) {
        std::optional<T> mean;
        mean = Pop();
        while (mean) {
            cb(mean.value());
            mean = Pop();
        }
    }

    ~MPSCStack() {
        DequeueAll([](const T&) {});
    }
};
