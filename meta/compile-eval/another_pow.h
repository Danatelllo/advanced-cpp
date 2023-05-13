#pragma once

constexpr int64_t pow(int a, int b) {
    return b != 0 ? static_cast<int64_t>(pow(a, b - 1)) * static_cast<int64_t>(a) : int64_t(1);
}
