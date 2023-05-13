#pragma once
#include <cmath>
#include <cstring>

#include <string>
constexpr int64_t pow(int a, int b, int mod) {
    return b != 0 ? ((static_cast<int64_t>(pow(a, b - 1, mod)) % mod) * (static_cast<int64_t>(a)) %
                     mod) %
                        mod
                  : int64_t(1);
}

constexpr int hash(const char *s, int p, int mod, int i = 0) {
    return s[i] == '\0' ? 0 : ((s[i] * (pow(p, i, mod))) % mod) + (hash(s, p, mod, i + 1) % mod);
}
