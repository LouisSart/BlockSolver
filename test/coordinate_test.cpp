#include "coordinate.hpp"

#include <cassert>
#include <iostream>

#include "cubie.hpp"

void init_array(unsigned* l, unsigned size, unsigned value) {
    for (unsigned i = 0; i < size; ++i) {
        l[i] = value;
    }
}

void print_array(const unsigned* l, unsigned size) {
    for (unsigned i = 0; i < size; ++i) {
        std::cout << l[i] << " ";
    }
    std::cout << std::endl;
}

void test_layout_coord() {
    unsigned check, size = NE, pieces = 5;
    unsigned layout[size];
    init_array(layout, size, 0);

    for (unsigned c = 0; c < binomial(size, pieces); ++c) {
        layout_from_coord(c, size, pieces, layout);
        check = layout_coord(layout, size);
        assert(check == c);
    }
}

void test_perm_coord() {
    constexpr unsigned n = 8;
    unsigned perm[n];

    for (unsigned c = 0; c < factorial(n); ++c) {
        perm_from_coord(c, n, perm);
        auto check = perm_coord(perm, n);
        assert(c == check);
    }
}

// void test_eo_coord() {
//     uint n{10};
//     uint eo[n] = {0, 1, 1, 0, 0, 1, 0, 1, 0, 1};
//     uint c = eo_coord(eo, n);
//     std::cout << c << '\n';
//     uint* eeo = eo_from_coord(c, n);
//     for (size_t i = 0; i < n; i++) {
//         std::cout << eeo[i] << ' ';
//     }
//     std::cout << '\n';
// }

// void test_co_coord() {
//     uint n{10};
//     uint co[n] = {0, 1, 2, 0, 2, 1, 0, 2, 0, 1};
//     uint c = co_coord(co, n);
//     std::cout << c << '\n';
//     uint* cco = co_from_coord(c, n);
//     for (size_t i = 0; i < n; i++) {
//         std::cout << cco[i] << ' ';
//     }
//     std::cout << '\n';
// }

int main() {
    test_layout_coord();
    test_perm_coord();
}