#pragma once
#include <array>
#include <cassert>
#include <iostream>
#include <vector>

constexpr int ipow(int k, unsigned n) {
    // integer power
    // computes ret = k^n
    // k can be negative hence using int
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

unsigned perm_coord(unsigned* perm, unsigned size) {
    unsigned c = 0;
    for (unsigned i = 1; i < size; ++i) {
        c *= (size - i + 1);
        for (unsigned j = i + 1; j <= size; ++j) {
            if (perm[i - 1] > perm[j - 1]) c += 1;
        }
    }
    return c;
}

unsigned layout_coord(unsigned* l, unsigned n) {
    // l: an array containing the positions of the
    // pieces, ex {0,1,1,0,0,1,0,0,0} for a 3-corner
    // layout (3 1s among 8 possible positions)
    // n: number of possible positions
    // (usually 12 for edges, 8 for corners)
    // "Solved position NEEDS to have all pieces on the left (c=0)
    // for the layout_from_coord function to work. Reorder the
    // ep/cp array if necessary" => NOT TRUE ?
    unsigned c{0}, x{0};
    for (unsigned i = 0; i < n; i++) {
        if (l[i] == 1) {
            c += binomial(i, x + 1);
            x += 1;
        }
    }
    return c;
}

unsigned co_coord(unsigned* co, unsigned n) {
    // co: the orientation array of the corners
    // n: the number of corners to compute  the coordinate from.
    // For a complete cube, coord is computed from only the first
    // 7 corners because the orientation of the last is forced by the others
    unsigned coord{0};
    for (size_t i = 0; i < n; i++) {
        coord += co[i] * ipow(3, i);
    }
    return coord;
}

unsigned eo_coord(unsigned* eo, unsigned n) {
    // eo: the orientation array of the edges
    // n: the number of edges to compute  the coordinate from.
    // For a complete cube, coord is computed from only the first
    // 11 edges because the orientation of the last is forced by the others
    unsigned coord{0};
    for (size_t i = 0; i < n; i++) {
        coord += eo[i] * ipow(2, i);
    }
    return coord;
}

void perm_from_coord(unsigned c, unsigned size, unsigned* perm) {
    perm[size - 1] = 0;
    for (unsigned i = size - 1; i > 0; --i) {
        perm[i - 1] = (c % (size - i + 1));
        c = c / (size - i + 1);
        for (auto j = i + 1; j <= size; ++j) {
            if (perm[j - 1] >= perm[i - 1]) {
                perm[j - 1] = perm[j - 1] + 1;
            }
        }
    }
}

void layout_from_coord(unsigned c, unsigned n, unsigned k, unsigned* l) {
    // c: the layout coordinate
    // n: number of possible positions (12 for edges, 8 for corners)
    // k: number of pieces
    // Builds the layout state that corresponds to c

    auto r = k;
    auto t = c;
    unsigned b;
    for (int i = n - 1; i >= 0; --i) {
        b = binomial(i, r);
        if (t >= b) {
            t = t - b;
            l[i] = 1;
            r = r - 1;
        } else {
            l[i] = 0;
        }
    }
}

void eo_from_coord(unsigned c, unsigned n, unsigned* eo) {
    // c: coordinate
    // n: number of edges of the block
    unsigned cc = c;
    for (unsigned i = 0; i < n; i++) {
        eo[i] = cc % 2;
        cc = cc / 2;
    }
}

void co_from_coord(unsigned c, unsigned n, unsigned* co) {
    // c: coordinate
    // n: number of corners in the block

    unsigned cc = c;
    for (unsigned i = 0; i < n; i++) {
        co[i] = cc % 3;
        cc = cc / 3;
    }
}

// Jaap Scherpuis implementation of the layout coordinate
// unsigned layout_index(unsigned* layout, unsigned size, unsigned pieces) {
//     unsigned t = 0;
//     auto r = pieces;
//     for (unsigned i = size - 1; i > 0; --i) {
//         if (layout[i] == 1) {
//             t += binomial(i, r);
//             r -= 1;
//         }
//     }
//     return t;
// }