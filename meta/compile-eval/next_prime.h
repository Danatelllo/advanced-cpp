#pragma once
#include <cmath>

constexpr int AlmostSqrt(int x) {
    int j = 2;
    while (j * j < x) {
        ++j;
    }
    return j;
}

constexpr bool IsPrime(int x) {
    if (x == 1) {
        return false;
    }
    if (x == 2) {
        return true;
    }
    int b = AlmostSqrt(x);
    for (int i = 2; i <= b; ++i) {
        if (x % i == 0) {
            return false;
        }
    }
    return true;
}

constexpr int next_prime(int x) {
    while (!IsPrime(x)) {
        ++x;
    }
    return x;
}
