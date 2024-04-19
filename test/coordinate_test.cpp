#include "coordinate.hpp"

#include <cassert>
#include <iostream>

#include "cubie.hpp"

// unsigned layout_index(unsigned* layout, unsigned size, unsigned pieces) {
//     unsigned t = 0;
//     auto r = pieces;
//     for (unsigned i = size - 1; i > 0; --i) {
//         if (layout[i] == 1) {
//             t = t + binomial(i, r);
//             r = r - 1;
//         }
//     }
//     return t;
// }

// void layout_from_index(unsigned index, unsigned size, unsigned pieces,
//                        unsigned* layout) {
//     auto r = pieces;
//     auto t = index;
//     for (int i = size - 1; i >= 0; --i) {
//         if (t >= binomial(i, r)) {
//             t = t - binomial(i, r);
//             layout[i] = 1;
//             r = r - 1;
//         } else {
//             layout[i] = 0;
//         }
//     }
// }

// void print_array(const unsigned* l, unsigned size) {
//     for (unsigned i = 0; i < size; ++i) {
//         std::cout << l[i] << " ";
//     }
//     std::cout << std::endl;
// }

void init_array(unsigned* l, unsigned size, unsigned value) {
    for (unsigned i = 0; i < size; ++i) {
        l[i] = value;
    }
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

// void test_perm_coord() {
//     uint n = 10;
//     uint perm[]{1, 2, 0, 4, 7, 5, 8, 9, 3, 6};
//     uint c = perm_coord(perm, n);
//     std::cout << c << '\n';
//     std::cout << "-------------" << '\n';

//     uint* p;
//     p = perm_from_coord(c, n);
//     for (size_t i = 0; i < n; i++) {
//         std::cout << *(p + i) << '\n';
//     }
// }

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

int main() { test_layout_coord(); }