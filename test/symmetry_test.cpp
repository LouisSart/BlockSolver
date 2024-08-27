#include "symmetry.hpp"

void test_index() {
    for (unsigned c = 0; c < N_SYM; ++c) {
        auto [i, j, k, l] = symmetry_index_to_num(c);
        assert(c == symmetry_index(i, j, k, l));
    }
}

void test_move_translation() {
    assert(S_URF_move_conj[U] == F);
    assert(z2_move_conj[R3] == L3);
    assert(y_move_conj[L3] == F3);
}

int main() {
    test_index();
    test_move_translation();
    std::cout << symmetry_move_conj(Algorithm{R, U, R3},
                                    symmetry_index(1, 0, 0, 0))
              << std::endl;
    std::cout << symmetry_move_conj(Algorithm{L, F, U},
                                    symmetry_index(0, 1, 0, 0))
              << std::endl;
    std::cout << symmetry_move_conj(Algorithm{B2, D3, L},
                                    symmetry_index(0, 0, 1, 0))
              << std::endl;
    std::cout << symmetry_move_conj(Algorithm{L3, R2, D},
                                    symmetry_index(0, 0, 0, 1))
              << std::endl;

    return 0;
}