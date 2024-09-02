#include "coordinate.hpp"

#include <cassert>
#include <iostream>

#include "cubie.hpp"
#include "utils.hpp"

void test_layout_index() {
    std::array<unsigned, NC> clayout;
    for (unsigned np = 0; np <= NC; ++np) {
        for (unsigned cl = 0; cl < binomial(NC, np); ++cl) {
            layout_from_index(cl, clayout, np);
            assert(layout_index(clayout, np) == cl);
        }
    }

    std::array<unsigned, NE> elayout;
    for (unsigned np = 0; np <= NE; ++np) {
        for (unsigned cl = 0; cl < binomial(NE, np); ++cl) {
            layout_from_index(cl, elayout, np);
            assert(layout_index(elayout, np) == cl);
        }
    }
}

void test_permutation_index() {
    std::array<unsigned, NC> cperm;

    for (unsigned c = 0; c < factorial(NC); ++c) {
        permutation_from_index(c, cperm);
        assert(c == permutation_index(cperm));
    }
}

void test_eo_index() {
    std::array<unsigned, NE> eo;

    for (unsigned c = 0; c < ipow(2, NE); ++c) {
        eo_from_index(c, eo);
        assert(c == eo_index(eo));
    }
}

void test_co_index() {
    std::array<unsigned, NC> co;

    for (unsigned c = 0; c < ipow(3, NC); ++c) {
        co_from_index(c, co);
        assert(c == co_index(co));
    }
}

int main() {
    test_layout_index();
    test_permutation_index();
    test_eo_index();
    test_co_index();
}