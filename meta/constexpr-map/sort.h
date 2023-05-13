#pragma once

#include <constexpr_map.h>
#include "constexpr_string.h"
#include <type_traits>

template <std::integral K, class V, int S>
constexpr auto Sort(ConstexprMap<K, V, S> map) {
    for (int i = 0; i < map.Size(); ++i) {
        for (int j = 1; j < map.Size(); ++j) {
            if (map.GetByIndex(j - 1).first < map.GetByIndex(j).first) {
                auto tmp = map.GetByIndex(j - 1);
                map.GetByIndex(j - 1) = map.GetByIndex(j);
                map.GetByIndex(j) = tmp;
            }
        }
    }
    return map;
}

template <class K, class V, int S>
constexpr auto Sort(ConstexprMap<K, V, S> map) {

    for (int i = 0; i < map.Size(); ++i) {
        for (int j = 1; j < map.Size(); ++j) {
            if (!(map.GetByIndex(j - 1).first < map.GetByIndex(j).first)) {
                auto tmp = map.GetByIndex(j - 1);
                map.GetByIndex(j - 1) = map.GetByIndex(j);
                map.GetByIndex(j) = tmp;
            }
        }
    }
    return map;
}
