#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t

#include "memory"

//#include "weak.h"

// std::nullptr_t

struct ControlBlock {
    size_t strong = 1;
    size_t weak = 0;
    bool is_deleted = false;
    virtual ~ControlBlock() = default;
    virtual void* Get() = 0;
    virtual void Set() = 0;
};

template <typename T>
struct ControlBlockPointer : public ControlBlock {
    ControlBlockPointer(){};

    explicit ControlBlockPointer(T* r) : ptr(r) {
    }

    ~ControlBlockPointer() override {
        if (ptr && !is_deleted) {
            delete ptr;
        }
    }
    void* Get() override {
        if (ptr != nullptr) {
            return ptr;
        }
    }
    void Set() override {
        if (ptr) {
            delete ptr;
        }
        is_deleted = true;
    }

    T* ptr = nullptr;
};

template <typename T>
struct ControlBlockEmplace : public ControlBlock {
    template <typename... Args>
    ControlBlockEmplace(Args&&... args) {
        new (&storage) T{std::forward<Args>(args)...};
    }

    ~ControlBlockEmplace() override {
        if (!is_deleted) {
            std::destroy_at(std::launder(reinterpret_cast<T*>(&storage)));
        }
    }

    void* Get() override {
        return reinterpret_cast<T*>(&storage);
    }

    void Set() override {
        std::destroy_at(std::launder(reinterpret_cast<T*>(&storage)));
        is_deleted = true;
    }

    std::aligned_storage_t<sizeof(T), alignof(T)> storage;
};

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>

class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr(){};
    SharedPtr(std::nullptr_t){};

    explicit SharedPtr(T* ptr) {
        block = new ControlBlockPointer<T>(ptr);
    }
    template <typename Y>
    SharedPtr(Y* ptr) {
        block = new ControlBlockPointer<Y>(ptr);
    }

    //    template<typename Y>
    //    SharedPtr(Y* ptr) {
    //        block = new ControlBlockPointer(ptr);
    //    }

    SharedPtr(ControlBlockEmplace<T>* other_block) {
        block = other_block;
    }

    SharedPtr(const SharedPtr<T>& other) {
        block = other.block;
        if (block != nullptr) {
            ++block->strong;
        }
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) {
        block = other.block;
        if (block != nullptr) {
            ++block->strong;
        }
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) {
        block = other.block;
        other.block = nullptr;
    }

    template <typename Y>
    SharedPtr<T>& operator=(const SharedPtr<Y>& other) {
        --block->strong;
        if (block->strong == 0 && block->weak == 0) {
            delete block;
        }
        block = other.block;
        if (block != nullptr) {
            ++block->strong;
        }
        return *this;
    }

    template <typename Y>
    SharedPtr<T>& operator=(SharedPtr<Y>&& other) {
        --block->strong;
        if (block->strong == 0) {
            delete block;
        }
        block = other.block;
        other.block = nullptr;
        return *this;
    }

    SharedPtr(SharedPtr&& other) {
        if (other.block != nullptr) {
            block = other.block;
            other.block = nullptr;
        }
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) {
        block = other.block;
        ++block->strong;
        r = ptr;
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.block->is_deleted || other.block == nullptr) {
            throw BadWeakPtr();
        }
        block = other.block;
        ++block->strong;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s
    SharedPtr& operator=(const SharedPtr& other) {
        if (Get() == other.Get()) {
            return *this;
        }
        if (other.block != nullptr) {
            if (block != nullptr) {
                --block->strong;
                if (block->strong == 0 && block->weak == 0) {
                    delete block;
                }
            }
            block = other.block;
            r = other.r;
            ++block->strong;
        }
        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) {
        if (other.block == nullptr) {
            return *this;
        } else {
            --block->strong;
            if (block->strong == 0 && block->weak == 0) {
                delete block;
            }
            block = other.block;
            other.block = nullptr;
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        if (block) {
            --block->strong;
            if (block->strong == 0 && block->weak == 0) {
                r = nullptr;
                delete block;
            } else if (block->strong == 0) {
                block->Set();
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block != nullptr) {
            --block->strong;
            if (block->strong == 0 && block->weak == 0) {
                delete block;
            }
        }
        block = nullptr;
    }

    void Reset(T* ptr) {
        if (Get() == ptr) {
            return;
        }
        if (block != nullptr) {
            --block->strong;
            if (block->strong == 0 && block->weak == 0) {
                delete block;
            }
            block = new ControlBlockPointer(ptr);
        } else {
            block = new ControlBlockPointer(ptr);
        }
    }
    template <typename Y>
    void Reset(Y* ptr) {
        if (Get() == ptr) {
            return;
        }
        if (block != nullptr) {
            --block->strong;
            if (block->strong == 0 && block->weak == 0) {
                delete block;
            }
            block = new ControlBlockPointer<Y>(ptr);
        } else {
            block = new ControlBlockPointer<Y>(ptr);
        }
    }
    void Swap(SharedPtr& other) {
        std::swap(block, other.block);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {

        if (r != nullptr) {
            return r;
        }
        if (block != nullptr) {
            return static_cast<T*>(block->Get());
        } else {
            return nullptr;
        }
    }
    T& operator*() const {
        if (r != nullptr) {
            return *r;
        }
        return *static_cast<T*>(block->Get());
    }
    T* operator->() const {
        return static_cast<T*>(block->Get());
    }
    size_t UseCount() const {
        if (block == nullptr) {
            return 0;
        }
        return block->strong;
    }
    explicit operator bool() const {
        return block != nullptr;
    }

    ControlBlock* block = nullptr;
    T* r = nullptr;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right);

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    auto block = new ControlBlockEmplace<T>(std::forward<Args>(args)...);
    return SharedPtr(block);
}

// Look for usage examples in tests
template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;

    WeakPtr<T> WeakFromThis() noexcept;
    WeakPtr<const T> WeakFromThis() const noexcept;
};
