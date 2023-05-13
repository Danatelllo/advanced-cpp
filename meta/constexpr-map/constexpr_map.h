#pragma once
#include <vector>
#include <array>

template <class K, class V, int MaxSize = 8>
class ConstexprMap {
public:
    constexpr ConstexprMap() = default;

    constexpr V& operator[](const K& key) {
        for (auto& node : map) {
            if (node.first == key) {
                return node.second;
            }
        }
        map[last].first = key;
        ++last;
        return map[last - 1].second;
    }

    constexpr const V& operator[](const K& key) const {
        for (auto& node : map) {
            if (node.first == key) {
                return node.second;
            }
        }
    }

    constexpr bool Erase(const K& key) {
        std::array<std::pair<K, V>, MaxSize> new_map;
        last = 0;
        bool is_erase = false;
        for (auto node : map) {
            if (key != node.first) {
                new_map[last] = node;
                ++last;
            } else {
                is_erase = true;
            }
        }
        map = new_map;
        return is_erase;
    }

    constexpr bool Find(const K& key) const {
        for (auto& node : map) {
            if (node.first == key) {
                return true;
            }
        }
        return false;
    }

    constexpr size_t Size() const {
        return last;
    }

    constexpr std::pair<K, V>& GetByIndex(size_t pos) {
        return map[pos];
    }

    constexpr const std::pair<K, V>& GetByIndex(size_t pos) const {
        return map[pos];
    }
    std::array<std::pair<K, V>, MaxSize> map;
    int last = 0;
};
