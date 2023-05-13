#pragma once

#include <unordered_map>
#include <mutex>
#include <functional>
#include <thread>
#include <list>
#include <error.h>
#include <atomic>

template <class K, class V, class Hash = std::hash<K>>
class ConcurrentHashMap {
public:
    ConcurrentHashMap(const Hash& hasher = Hash()) : ConcurrentHashMap(kUndefinedSize, hasher) {
    }

    explicit ConcurrentHashMap(int expected_size, const Hash& hasher = Hash())
        : ConcurrentHashMap(expected_size, kDefaultConcurrencyLevel, hasher) {
    }

    ConcurrentHashMap(int expected_size, int expected_threads_count, const Hash& hasher = Hash()) {
        if (expected_size != kUndefinedSize) {
            table_.resize(expected_size);
            mutex_ = std::vector<std::mutex>(expected_size);
        } else {
            table_.resize(kUndefinedSize);
            mutex_ = std::vector<std::mutex>(kUndefinedSize);
        }
        hash_ = hasher;
    }

    size_t GetHash(const K& key) const {
        return hash_(key);
    }
    size_t GetPlaceForInsert(size_t meaning_hash, size_t size) const {
        return meaning_hash % size;
    }

    bool Insert(const K& key, const V& value) {
        size_t hash_meaning = GetHash(key);
        size_t place_for_insert = GetPlaceForInsert(hash_meaning, table_.size());
        auto result = Find(key);

        {
            std::lock_guard<std::mutex> lock(mutex_[place_for_insert]);
            if (result.first) {
                return false;
            }
            ++size_;
            table_[place_for_insert].emplace_back(key, value);
        }
        return true;
    }

    bool Erase(const K& key) {
        size_t hash_meaning = GetHash(key);
        size_t place_for_insert = GetPlaceForInsert(hash_meaning, table_.size());
        bool is_erase = false;
        {
            std::lock_guard<std::mutex> lock(mutex_[place_for_insert]);
            for (auto it = table_[place_for_insert].begin(); it != table_[place_for_insert].end();
                 ++it) {
                if (it->first == key) {
                    table_[place_for_insert].erase(it);
                    is_erase = true;
                    --size_;
                    break;
                }
            }
        }
        return is_erase;
    }
    void Clear() {
        for (size_t j = 0; j < mutex_.size(); ++j) {
            std::lock_guard<std::mutex> lock{mutex_[j]};
            table_[j].clear();
        }
        size_ = 0;
    }

    std::pair<bool, V> Find(const K& key) const {
        V value = V();
        bool is_find = false;
        const size_t hash_meaning = GetHash(key);
        const size_t place_for_insert = GetPlaceForInsert(hash_meaning, table_.size());

        {
            std::lock_guard<std::mutex> lock(mutex_[place_for_insert]);
            for (auto it = table_[place_for_insert].begin(); it != table_[place_for_insert].end();
                 ++it) {
                if (it->first == key) {
                    is_find = true;
                    value = it->second;
                }
            }
        }
        return std::pair(is_find, value);
    }

    const V At(const K& key) const {
        auto result = Find(key);
        //???????
        if (!result.first) {
            throw std::out_of_range("bad key");
        }
        return result.second;
    }

    size_t Size() const {
        return size_;
    }

    static const int kDefaultConcurrencyLevel;
    static const int kUndefinedSize;

private:
    std::vector<std::vector<std::pair<K, V>>> table_;
    Hash hash_;
    mutable std::vector<std::mutex> mutex_;
    std::atomic<size_t> size_;
    //    std::unordered_map<K, V, Hash> table_;
    //    mutable std::mutex mutex_;
};

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kDefaultConcurrencyLevel = 8;

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kUndefinedSize = 800;
