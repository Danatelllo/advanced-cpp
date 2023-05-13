#pragma once

#include "sw_fwd.h"  // Forward declaration
#include "shared.h"

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() {
    }

    WeakPtr(const WeakPtr& other) {
        block = other.block;
        r = other.r;
        if (block != nullptr) {
            ++other.block->weak;
        }
    }
    WeakPtr(WeakPtr&& other) {
        block = other.block;
        r = other.r;
        other.block = nullptr;
        other.r = nullptr;
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T>& other) {
        if (other.block) {
            block = other.block;
            ++block->weak;
            r = other.r;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        if (block == other.block) {
            return *this;
        }
        if (block != nullptr) {
            --block->weak;
            if (block->strong == 0 && block->weak == 0) {
                delete block;
            }
        }
        if (other.block != nullptr) {
            block = other.block;
            r = other.r;
            ++block->weak;
        } else {
            block = other.block;
            r = other.r;
        }
        return *this;
    }
    WeakPtr& operator=(WeakPtr&& other) {
        if (other.block == block) {
            return *this;
        }
        if (block) {
            --block->weak;
            if (block->weak == 0 && block->strong == 0 && block != nullptr) {
                delete block;
            }
        }
        block = other.block;
        r = other.r;
        other.block = nullptr;
        other.r = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        if (block) {
            --block->weak;
            if (block->weak == 0 && block->strong == 0) {
                delete block;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block != nullptr) {
            --block->weak;
            if (block->strong == 0 && block->weak == 0) {
                delete block;
            }
        }
        block = new ControlBlockPointer<T>();
        block->strong = 0;
        ++block->weak;
    }
    void Swap(WeakPtr& other) {
        std::swap(block, other.block);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const {
        return block->strong;
    }
    bool Expired() const {
        if (!block) {
            return 1;
        }
        return block->strong == 0;
    }
    SharedPtr<T> Lock() const {
        if (this->Expired()) {
            return SharedPtr<T>();
        }
        if (block) {
            ++block->strong;
            auto tmp = SharedPtr<T>();
            tmp.block = block;
            return tmp;
        } else {
            return SharedPtr<T>();
        }
    }

    ControlBlock* block = nullptr;
    T* r = nullptr;
};
