#include <array>

template <int N>
constexpr int& GetElement(std::array<std::array<int, N>, N>& a, int i, int j) {
    return (&std::get<0>(((&std::get<0>(a))[i])))[j];
}

template <int N>
constexpr int determinant(std::array<std::array<int, N>, N> a, int size = N - 1) {
    if (size == 0) {
        return GetElement<N>(a, 0, 0);
    }
    int pred = 0;
    int answer = 0;
    for (int j = 0; j < size + 1; ++j) {
        std::array<std::array<int, N>, N> b{};
        for (int i = 0; i < size; ++i) {
            for (int k = 0; k < size; ++k) {
                pred = 0;
                if (k >= j) {
                    pred = 1;
                }
                GetElement<N>(b, i, k) = GetElement<N>(a, i + 1, k + pred);
            }
        }
        auto l = determinant<N>(b, size - 1);
        int c = GetElement<N>(a, 0, j);
        if (j % 2 == 0) {
            answer += c * l;
        } else {
            answer += -1 * c * l;
        }
    }
    return answer;
}