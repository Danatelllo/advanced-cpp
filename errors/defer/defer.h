#pragma once
#include "iostream"
#include "memory"
#include "type_traits"

template <typename Callback>
class CallbackStorage {
public:
    //    explicit CallbackStorage(Callback callback) {
    //        new(&callback_buffer_) Callback(std::move(callback));
    //    }

    //    ~CallbackStorage() {
    //        std::destroy_at(std::launder(&callback_buffer_));
    ////            std::destroy_at(reinterpret_cast<Callback*>(callback_buffer_));
    //    }
    //    Callback& GetCallback() {
    //        return *reinterpret_cast<Callback*>(&callback_buffer_);
    //    }
    //
    explicit CallbackStorage(Callback callback) {
        ::new (GetCallbackBuffer()) Callback(std::move(callback));
    }

    void* GetCallbackBuffer() {
        return static_cast<void*>(callback_buffer_);
    }

    Callback& GetCallback() {
        return *reinterpret_cast<Callback*>(GetCallbackBuffer());
    }
    alignas(Callback) char callback_buffer_[sizeof(Callback)];
};

template <typename Callback>
class Defer final {
public:
    Defer() = default;

    Defer(Callback&& other) : block(CallbackStorage<Callback>(std::forward<Callback>(other))) {
    }

    ~Defer() {
        if (!flag) {
            std::move(block.GetCallback())();
            std::destroy_at(std::launder(&block.callback_buffer_));
        }
    }

    void Cancel() {
        std::destroy_at(reinterpret_cast<Callback*>(block.callback_buffer_));
        flag = true;
    }

    void Invoke() {
        std::move(block.GetCallback())();
        flag = true;
    }

    CallbackStorage<Callback> block;
    bool flag = false;
};
