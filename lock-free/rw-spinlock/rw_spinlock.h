#pragma once

#include <atomic>
int Excgh(int expected, int desired, std::atomic<int> *value) {
    int *answer = &expected;
    std::atomic_compare_exchange_strong(value, answer, desired);
    return *answer;
}
struct RWSpinLock {

    void LockRead() {
        //        int des = atomic_.load() - (atomic_.load() % 2) + 2;
        //        int exp = des - 2;
        //        while (!atomic_.compare_exchange_weak(exp, des)) {
        //            exp = atomic_.load() - (atomic_.load() % 2);
        //            des = atomic_.load() + 2;
        //        };
        while (atomic_ % 2 == 1) {
        };
        atomic_.fetch_add(2);
    }

    void UnlockRead() {
        atomic_.fetch_sub(2);
    }

    void LockWrite() {
        int exp = 0;
        int des = 1;
        while (Excgh(exp, des, &atomic_)) {
            //            exp = 0;
        };
    }

    void UnlockWrite() {
        atomic_.fetch_sub(1);
    }

private:
    std::atomic<int> atomic_ = 0;
};
