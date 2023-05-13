#pragma once

#include <utility>
#include <functional>

template <typename Signature>
class FunctionRef;

template <typename Result, typename... Args>
class FunctionRef<Result(Args...)> {
public:
    template <typename Function>
    FunctionRef(Function&& function) {
        FPointer = reinterpret_cast<void*>(&function);
        Rfunction = [](void* pointer, Args&&... args) -> Result {
            //            if (std::is_same_v<Function, void>)
            //                return (*reinterpret_cast<std::add_pointer_t<Function>>(pointer))(
            //                    std::forward<Args>(args)...);
            return static_cast<Result>((*reinterpret_cast<std::add_pointer_t<Function>>(pointer))(
                std::forward<Args>(args)...));
        };
    }

    Result operator()(Args&&... args) {
        return Rfunction(FPointer, std::forward<Args>(args)...);
    }

private:
    void* FPointer;
    Result (*Rfunction)(void*, Args&&...);
};