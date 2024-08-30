#include "symmetry.hpp"

#include "cubie_cube.hpp"

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

void test_symmetries() {
    // Testing that S^-1 * c * S * m == S^-1 * (c * m') * S
    // with c a random CubieCube for all S, m
    // CubieCube random = CubieCube::random_state();
    CubieCube random;

    for (unsigned s = 0; s < N_SYM; ++s) {
        for (Move m : HTM_Moves) {
            CubieCube cc1 = random.get_conjugate(s);
            cc1.apply(m);

            CubieCube cc2 = random;
            cc2.apply(move_conj(m, s));
            cc2 = cc2.get_conjugate(s);

            assert(cc1 == cc2);
        }
    }
}

int main() {
    test_index();
    test_move_translation();
    test_symmetries();
    return 0;
}