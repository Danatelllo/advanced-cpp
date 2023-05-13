#include "find_subsets.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <cmath>
#include <mutex>

bool is_begin = true;
std::atomic<bool> exist = false;
std::mutex mtx;

void CheckAnswer(std::vector<int> &v1, std::vector<int> &v2, const std::vector<int64_t> &data,
                 Subsets &answer) {
    std::vector<size_t> first;
    std::vector<size_t> second;
    for (size_t j = 0; j < v1.size(); ++j) {
        if (v1[j] > 0) {
            if (static_cast<size_t>(v1[j]) == data.size()) {
                first.push_back(0);
            } else {
                first.push_back(v1[j]);
            }
        } else {
            if (v1[j] == -static_cast<int>(data.size())) {
                second.push_back(0);
            } else {
                second.push_back(-v1[j]);
            }
        }
    }
    for (size_t j = 0; j < v2.size(); ++j) {
        if (v2[j] < 0) {
            if (-v2[j] == static_cast<int>(data.size())) {
                first.push_back(0);
            } else {
                first.push_back(-v2[j]);
            }
        } else {
            if (v2[j] == static_cast<int>(data.size())) {
                second.push_back(0);
            } else {
                second.push_back(v2[j]);
            }
        }
    }
    if (!first.empty() && !second.empty()) {
        exist = true;

        {
            std::lock_guard<std::mutex> lock{mtx};
            answer.second_indices = second;
            answer.first_indices = first;
            answer.exists = true;
        }
    }
}

std::pair<std::vector<int>, int> GetTernaryVectorAndSum(int number,
                                                        const std::vector<int64_t> &data,
                                                        int counter) {
    std::vector<int> result;
    int sum = 0;
    while (number != 0) {
        int k = number % 3;
        if (k == 1) {
            if (is_begin) {
                if (counter == 0) {
                    result.push_back(data.size());
                } else {
                    result.push_back(counter);
                }
                sum += data[counter];
            } else {
                if (counter == 0) {
                    result.push_back(-data.size());
                } else {
                    result.push_back(-counter);
                }
                sum -= data[counter];
            }

        } else if (k == 2) {
            if (is_begin) {
                if (counter == 0) {
                    result.push_back(-data.size());
                } else {
                    result.push_back(-counter);
                }
                sum -= data[counter];
            } else {
                if (counter == 0) {
                    result.push_back(data.size());
                } else {
                    result.push_back(counter);
                }
                sum += data[counter];
            }
        }
        ++counter;
        number /= 3;
    }
    return std::pair(result, sum);
}

void FindSums(std::unordered_map<int, std::vector<int>> &sums, int begin, int end, size_t step,
              const std::vector<int64_t> &data, Subsets &answer, size_t k) {
    for (int i = begin; i < end; i += step) {
        if (is_begin) {
            auto result = GetTernaryVectorAndSum(i, data, 0);
            sums[result.second] = result.first;
        } else {
            if (exist) {
                return;
            }
            // от 0 до k - 1 и от k до n не вкл
            auto result = GetTernaryVectorAndSum(i, data, k);
            if (sums.contains(result.second)) {
                CheckAnswer(sums[result.second], result.first, data, answer);
            }
        }
    }
}

Subsets FindEqualSumSubsets(const std::vector<int64_t> &data) {
    is_begin = true;
    size_t k = data.size() / 2;
    exist = false;
    Subsets answer;
    answer.exists = false;
    std::unordered_map<int, std::vector<int>> sums;
    FindSums(sums, 0, std::pow(3, k), 1, data, answer, 0);
    size_t counter_threads =
        std::min(static_cast<size_t>(std::thread::hardware_concurrency()), data.size() - k);

    int end = std::pow(3, data.size() - k);
    is_begin = false;

    std::vector<std::thread> threads;

    for (size_t j = 0; j < counter_threads; ++j) {
        threads.emplace_back(FindSums, std::ref(sums), j, end, counter_threads, data,
                             std::ref(answer), k);
    }
    for (auto &t : threads) {
        t.join();
    }
    return answer;
}
