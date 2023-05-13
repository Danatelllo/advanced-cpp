#pragma once
#include "memory"

class Any {
private:
    class ContainerBase {
    public:
        ContainerBase() = default;
        virtual ~ContainerBase() = default;
        virtual const std::type_info& Type() const = 0;
        virtual const std::unique_ptr<ContainerBase> NewVersion() const = 0;
    };

    template <typename T>
    class Container : public ContainerBase {
    public:
        const std::unique_ptr<ContainerBase> NewVersion() const override {
            return std::make_unique<Container>(Container(data_));
        }

        Container() = default;

        Container(const T& value) {
            data_ = value;
        }
        const T& Get() const {
            return data_;
        }
        const std::type_info& Type() const override {
            return typeid(data_);
        }

    private:
        T data_;
    };

public:
    Any() {
    }

    template <class T>
    Any(const T& value) {
        ptr_ = std::make_unique<Container<T>>(Container(value));
    }

    template <class T>
    Any& operator=(const T& value) {
        ptr_ = std::make_unique<Container<T>>(Container(value));
        return *this;
    }

    Any(const Any& rhs) {
        auto b = rhs.ptr_->NewVersion();
        std::swap(ptr_, b);
    }
    Any& operator=(const Any& rhs) {
        if (ptr_ == rhs.ptr_) {
            return *this;
        }
        auto b = rhs.ptr_->NewVersion();
        std::swap(ptr_, b);
        return *this;
    }
    ~Any() {
    }

    bool Empty() const {
        return ptr_ == nullptr;
    }

    void Clear() {
        ptr_ = nullptr;
    }
    void Swap(Any& rhs) {
        std::swap(ptr_, rhs.ptr_);
    }

    template <class T>
    const T& GetValue() const {
        if (ptr_->Type() != typeid(T)) {
            throw std::bad_cast();
        }
        return reinterpret_cast<Container<T>*>(ptr_.get())->Get();
    }
    std::unique_ptr<ContainerBase> ptr_ = nullptr;
};