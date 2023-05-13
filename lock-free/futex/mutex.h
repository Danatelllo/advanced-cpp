#pragma once

#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#include <atomic>
int Excgh(int expected, int desired, std::atomic<int> *value) {
    int *answer = &expected;
    std::atomic_compare_exchange_strong(value, answer, desired);
    return *answer;
}
// Atomically do the following:
//    if (*value == expected_value) {
//        sleep_on_address(value)
//    }
void FutexWait(int *value, int expected_value) {
    syscall(SYS_futex, value, FUTEX_WAIT_PRIVATE, expected_value, nullptr, nullptr, 0);
}

// Wakeup 'count' threads sleeping on address of value(-1 wakes all)
void FutexWake(int *value, int count) {
    syscall(SYS_futex, value, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr, 0);
}

class Mutex {
public:
    void Lock() {
        int c = Excgh(0, 1, &value_);
        if (c != 0) {
            do {
                if (c == 2 || Excgh(1, 2, &value_) != 0) {
                    FutexWait(reinterpret_cast<int *>(&value_), c + 1);
                }
            } while ((c = Excgh(0, 2, &value_)) != 0);
        }
    }

    void Unlock() {
        if (value_.fetch_sub(1) != 1) {
            value_.store(0);
            FutexWake(reinterpret_cast<int *>(&value_), 1);
        }
    }

private:
    std::atomic<int> value_ = 0;
};
