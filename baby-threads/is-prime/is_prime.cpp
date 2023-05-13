#include "is_prime.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>
#include <atomic>

std::atomic<bool> is_true = true;
void CheckForThread(uint64_t x, unsigned long long step, unsigned long long begin) {
    if (x <= 1) {
        is_true.store(false);
        return;
    }
    uint64_t root = sqrt(x);
    unsigned long long bound = std::min(root + 6, x);
    for (unsigned long long i = begin; i < bound; i += step) {
        if (!is_true.load()) {
            return;
        }
        if (x % i == 0) {
            is_true.store(false);
            return;
        }
    }
}

bool IsPrime(uint64_t x) {
    std::vector<std::thread> threads;
    is_true.store(true);
    unsigned long long step = std::thread::hardware_concurrency();
    for (unsigned long long i = 2; i < step + 2; ++i) {
        threads.emplace_back(CheckForThread, x, step, i);
    }
    for (std::thread& t : threads) {
        t.join();
    }
    return is_true;
}
