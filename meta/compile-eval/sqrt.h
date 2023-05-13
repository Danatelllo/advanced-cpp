#include <iostream>

template <bool B, class T, class F>
struct conditional {
    typedef T type;
};

template <class T, class F>
struct conditional<false, T, F> {
    typedef F type;
};

template <int begin, int end>
struct Asnwer {

    enum { value = begin };
};
// conditional<(static_cast<long>((end + begin) / 2) * static_cast<long>((end + begin) / 2) >=
//             static_cast<long>(N)),
//
//            BinarySearch<((end + begin) / 2) + 1, end, N>,
//            BinarySearch<begin, ((end + begin) / 2) - 1, N>>,
//    typename Asnwer<begin, end>>;
template <int begin, int end, int N>
struct BinarySearch {
    enum {
        value =
            conditional<(begin <= end),
                        typename conditional<static_cast<long>((end + begin) / 2) *
                                                     static_cast<long>((end + begin) / 2) >=
                                                 static_cast<long>(N),
                                             BinarySearch<begin, ((end + begin) / 2) - 1, N>,
                                             BinarySearch<((end + begin) / 2) + 1, end, N> >::type,
                        Asnwer<begin, end> >::type::value
    };
};
template <int N>
struct Sqrt {
    enum { value = BinarySearch<0, N, N>::value };
};

// template<>
// struct Sqrt<1> {
//    static const int value = 1;
//};
