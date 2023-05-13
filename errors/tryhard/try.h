//#pragma once
//
//#include <exception>
//#include <stdexcept>
//#include <iostream>
//#include "cstring"
//
//
// class My_exception : public std::exception {
// public:
//    My_exception() {};
//    std::exception ex;
//    std::string message;
//
//     const char* what() const noexcept override {
//        return message.c_str();
//    }
//};
//
// template <class T>
// class Try {
// public:
//    Try() = default;
//
//    Try(T other) {
//        obj = other;
//        obj_is_empty = false;
//    }
//
//    Try(std::exception&& ex) {
//        e.ex = ex;
//        e.message = ex.what();
//        is_exception = true;
//    }
//
//
//    bool IsFailed() {
//        return is_exception;
//    }
//
//    auto Throw() {
//        if (is_exception) {
//            throw e;
//        }
//        throw std::runtime_error("No exception");
//    }
//
//    auto Value() {
//        if (is_exception) {
//            throw e;
//        }
//        if (obj_is_empty) {
//            throw std::runtime_error("Object is empty");
//        }
//        return obj;
//    }
//
//    My_exception e;
//    bool is_exception = false;
//    bool obj_is_empty = true;
//    T obj;
//
//
//};
//
// template <>
// class Try<void> {
// public:
//    Try() = default;
//
//
//    Try(std::exception&& ex) {
//        e.ex = ex;
//        e.message = ex.what();
//        is_exception = true;
//    }
//
//    bool IsFailed() {
//        return is_exception;
//    }
//
//    auto Throw() {
//        if (is_exception) {
//            throw e;
//        }
//        throw std::runtime_error("No exception");
//    }
//
//
//    My_exception e;
//    bool is_exception = false;
//    bool obj_is_empty = true;
//
//};
//
// template <class Function, class... Args>
// auto TryRun(Function func, Args... args) {
//    using ReturnType = decltype(func(args...));
//
//    if constexpr (!std::is_void_v<ReturnType>) {
//        try {
//            func(args...);
//        } catch (std::exception& ex) {
//            return Try<ReturnType>(std::move(ex));
//        } catch (const char* str) {
//            return Try<ReturnType>(std::runtime_error(str));
//        } catch (int str) {
//            return Try<ReturnType>(std::runtime_error(std::strerror(str)));
//        } catch(...) {
//            return Try<ReturnType>(std::runtime_error("Unknown exception"));
//        }
//        return Try<ReturnType>(func(args...));
//    }
//    try {
//        func(args...);
//    } catch(std::exception& ex) {
//        return Try<ReturnType>(std::move(ex));
//    } catch (const char* str) {
//        return Try<ReturnType>(std::runtime_error(str));
//    } catch (int str) {
//        return Try<ReturnType>(std::runtime_error(std::strerror(str)));
//    } catch (...) {
//        return Try<ReturnType>(std::runtime_error("Unknown exception"));
//    }
//    return Try<ReturnType>();
//}
//
#pragma once

#include <exception>
#include <stdexcept>
#include <iostream>
#include "cstring"

template <class T>
class Try {
public:
    Try() = default;

    Try(T other) {
        obj = other;
        obj_is_empty = false;
    }

    template <typename exc>
    Try(const exc& ex) {
        try {
            throw ex;
        } catch (...) {
            e = std::current_exception();
        }
        is_exception = true;
    }

    bool IsFailed() {
        return is_exception;
    }

    auto Throw() {
        if (is_exception) {
            std::rethrow_exception(e);
        }
        throw std::runtime_error("No exception");
    }

    auto Value() {
        if (is_exception) {
            std::rethrow_exception(e);
        }
        if (obj_is_empty) {
            throw std::runtime_error("Object is empty");
        }
        return obj;
    }

    Try(std::exception_ptr&& other) {
        std::swap(e, other);
    }

    std::exception_ptr e;
    bool is_exception = false;
    bool obj_is_empty = true;
    T obj;
};

template <>
class Try<void> {
public:
    Try() = default;

    template <typename exc>
    Try(const exc& ex) {
        try {
            throw ex;
        } catch (...) {
            e = std::current_exception();
        }
        is_exception = true;
    }

    bool IsFailed() {
        return is_exception;
    }

    auto Throw() {
        if (is_exception) {
            std::rethrow_exception(e);
        }
        throw std::runtime_error("No exception");
    }

    Try(std::exception_ptr&& other) {
        std::swap(e, other);
        other = nullptr;
        is_exception = true;
    }

    std::exception_ptr e;
    bool is_exception = false;
    bool obj_is_empty = true;
};

template <class Function, class... Args>
auto TryRun(Function func, Args... args) {
    using ReturnType = decltype(func(args...));

    if constexpr (!std::is_void_v<ReturnType>) {
        try {
            func(args...);
        } catch (std::exception& ex) {
            auto a = std::current_exception();
            return Try<ReturnType>(ex);
        } catch (const char* str) {
            return Try<ReturnType>(std::runtime_error(str));
        } catch (int str) {
            return Try<ReturnType>(std::runtime_error(std::strerror(str)));
        } catch (...) {
            return Try<ReturnType>(std::runtime_error("Unknown exception"));
        }
        return Try<ReturnType>(func(args...));
    }
    try {
        func(args...);
    } catch (std::exception& ex) {
        return Try<ReturnType>(std::move(std::current_exception()));
    } catch (const char* str) {
        return Try<ReturnType>(std::runtime_error(str));
    } catch (int str) {
        return Try<ReturnType>(std::runtime_error(std::strerror(str)));
    } catch (...) {
        return Try<ReturnType>(std::runtime_error("Unknown exception"));
    }
    return Try<ReturnType>();
}
