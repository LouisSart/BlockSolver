#pragma once
#include <array>
#include <cassert>
#include <iostream>
#include <vector>

constexpr unsigned ipow(unsigned k, unsigned n) {
    // unsigned integer power
    // computes ret = k^n
    unsigned ret{1};
    for (unsigned i = 0; i < n; i++) {
        ret *= k;
    }
    return ret;
};

constexpr auto factorial_table = [] {
    std::array<unsigned, 13> arr = {};
    arr[0] = 1;
    for (unsigned n = 1; n <= 12; ++n) {
        arr[n] = n * arr[n - 1];
    }
    return arr;
}();

constexpr unsigned factorial(unsigned n) {
    assert(n <= 12);
    return factorial_table[n];
}

constexpr auto binomial_table = [] {
    // Using constexpr lambda to fill up binomial table
    // at compile time
    std::array<unsigned, (13) * (13)> arr = {};
    for (unsigned n = 0; n <= 12; ++n) {
        for (unsigned k = 0; k <= 12; ++k) {
            if (n < k) {
                arr[n * 13 + k] = 0;
            } else if (k == 0 || k == n) {
                arr[n * 13 + k] = 1;
            } else {
                arr[n * 13 + k] =
                    arr[(n - 1) * 13 + k - 1] + arr[(n - 1) * 13 + k];
            }
        }
    }
    return arr;
}();

constexpr unsigned binomial(unsigned n, unsigned k) {
    // The function just does a lookup in the table for better performance
    assert(n >= 0 && k >= 0);  // "No negative values"
    assert(n < 13 && k < 13);  // "Binomial numbers computed up to n=12"
    return binomial_table[n * 13 + k];
}

template <std::size_t n>
unsigned permutation_index(const std::array<unsigned, n>& perm) {
    // perm: an array of strictly disctinct values
    // usually a permutation of {0, 1, ..., n-1}
    unsigned c = 0;
    for (unsigned i = 1; i < n; ++i) {
        c *= (n - i + 1);
        for (unsigned j = i + 1; j <= n; ++j) {
            if (perm[i - 1] > perm[j - 1]) c += 1;
        }
    }
    return c;
}

template <std::size_t n>
unsigned layout_index(const std::array<unsigned, n>& layout, unsigned r) {
    // n: number of positions
    // r: number of pieces
    unsigned t = 0;
    for (unsigned i = n - 1; i > 0; --i) {
        if (layout[i] == 1) {
            t += binomial(i, r);
            r -= 1;
        }
    }
    return t;
}

template <std::size_t n, bool drop_last = false>
unsigned co_index(const std::array<unsigned, n>& co) {
    // co: the orientation array of the corners
    // n: the number of corners to compute  the coordinate from
    // drop_last: if true, ignore last piece in coordinate computation
    constexpr unsigned drop = drop_last ? 1 : 0;
    unsigned c = 0;
    for (size_t i = 0; i < n - drop; i++) {
        c += co[i] * ipow(3, i);
    }
    return c;
}

template <std::size_t n, bool drop_last = false>
unsigned eo_index(const std::array<unsigned, n>& eo) {
    // eo: the orientation array of the edges
    // n: the number of edges to compute  the coordinate from
    // drop_last: if true, ignore last piece in coordinate computation
    constexpr unsigned drop = drop_last ? 1 : 0;
    unsigned c = 0;
    for (size_t i = 0; i < n - drop; i++) {
        c += eo[i] * ipow(2, i);
    }
    return c;
}

template <std::size_t n>
void permutation_from_index(unsigned c, std::array<unsigned, n>& perm) {
    perm[n - 1] = 0;
    for (unsigned i = n - 1; i > 0; --i) {
        perm[i - 1] = (c % (n - i + 1));
        c = c / (n - i + 1);
        for (auto j = i + 1; j <= n; ++j) {
            if (perm[j - 1] >= perm[i - 1]) {
                perm[j - 1] = perm[j - 1] + 1;
            }
        }
    }
}

template <std::size_t n>
void layout_from_index(unsigned c, std::array<unsigned, n>& layout,
                       unsigned r) {
    // n: number of positions
    // r: number of pieces
    assert(r <= n);
    assert(c < binomial(n, r));
    for (int i = n - 1; i >= 0; --i) {
        if (c >= binomial(i, r)) {
            c = c - binomial(i, r);
            layout[i] = 1;
            r = r - 1;
        } else {
            layout[i] = 0;
        }
    }
}

template <std::size_t n, bool drop_last = false>
void eo_from_index(unsigned c, std::array<unsigned, n>& eo) {
    // c: coordinate
    // n: number of edges
    // eo: the array with the individual orientations of the n edges
    // drop_last: if true, ignore last piece in coordinate computation
    constexpr unsigned drop = drop_last ? 1 : 0;
    for (unsigned i = 0; i < n - drop; i++) {
        eo[i] = c % 2;
        c = c / 2;
    }
}

template <std::size_t n, bool drop_last = false>
void co_from_index(unsigned c, std::array<unsigned, n>& co) {
    // c: coordinate
    // n: number of corners in the block
    // co: the array with the individual orientations of the n corners
    // drop_last: if true, ignore last piece in coordinate computation
    constexpr unsigned drop = drop_last ? 1 : 0;
    for (unsigned i = 0; i < n - drop; i++) {
        co[i] = c % 3;
        c = c / 3;
    }
}