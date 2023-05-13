#pragma once
#include "iostream"
#include "vector"

template <typename T, typename P, typename F>
void TransformIf(T begin, T end, const P& p, const F& f) {
    std::vector<std::pair<T, typeof(*begin)>> last_version;
    for (auto i = begin; i != end; ++i) {
        try {
            if (p(*i)) {
                try {
                    last_version.emplace_back(i, *i);
                } catch (...) {
                };
                try {
                    f(*i);
                } catch (...) {
                    for (auto& ch : last_version) {
                        *ch.first = ch.second;
                    }
                    throw;
                }
            }
        } catch (...) {
            for (auto& ch : last_version) {
                *ch.first = ch.second;
            }
            throw;
        }
    }
}
