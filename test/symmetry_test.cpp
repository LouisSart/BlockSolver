#include "symmetry.hpp"

#include "cubie_cube.hpp"

void test_index() {
    for (unsigned c = 0; c < N_SYM; ++c) {
        auto [i, j, k, l] = symmetry_index_to_num(c);
        assert(c == symmetry_index(i, j, k, l));
    }
}

void test_move_translation() {
    assert(S_URF_move_conj[U] == R);
    assert(z2_move_conj[R3] == L3);
    assert(y_move_conj[L3] == B3);
}

void test_move_permutation() {
    auto test_moves = HTM_Moves;

    for (unsigned s = 0; s < N_SYM; ++s) {
        auto [c_surf, c_y, c_z2, c_lr] = symmetry_index_to_num(s);

        // Permute one way
        for (unsigned k_lr = 0; k_lr < c_lr; ++k_lr) {
            permute_moves(test_moves.data(), LR_mirror_move_conj);
        }
        for (unsigned k_z2 = 0; k_z2 < c_z2; ++k_z2) {
            permute_moves(test_moves.data(), z2_move_conj);
        }
        for (unsigned k_y = 0; k_y < c_y; ++k_y) {
            permute_moves(test_moves.data(), y_move_conj);
        }
        for (unsigned k_surf = 0; k_surf < c_surf; ++k_surf) {
            permute_moves(test_moves.data(), S_URF_move_conj);
        }

        // Permute backwards
        for (unsigned k_surf = 0; k_surf < (3 - c_surf) % 3; ++k_surf) {
            permute_moves(test_moves.data(), S_URF_move_conj);
        }
        for (unsigned k_y = 0; k_y < (4 - c_y) % 4; ++k_y) {
            permute_moves(test_moves.data(), y_move_conj);
        }
        for (unsigned k_z2 = 0; k_z2 < c_z2; ++k_z2) {
            permute_moves(test_moves.data(), z2_move_conj);
        }
        for (unsigned k_lr = 0; k_lr < c_lr; ++k_lr) {
            permute_moves(test_moves.data(), LR_mirror_move_conj);
        }
        assert(test_moves == HTM_Moves);
    }
}

void test_symmetries() {
    // Testing that S^-1 * c * S * m == S^-1 * (c * m') * S
    // with c a random CubieCube for all S, m
    // CubieCube random = CubieCube::random_state();
    CubieCube random;

    for (unsigned s = 0; s < N_SYM; ++s) {
        for (Move m : HTM_Moves) {
            CubieCube cc1 = random.get_conjugate(s);
            cc1.apply(move_conj(m, s));

            CubieCube cc2 = random;
            cc2.apply(m);
            cc2 = cc2.get_conjugate(s);

            assert(cc1 == cc2);
        }
    }
}

int main() {
    test_index();
    test_move_translation();
    test_move_permutation();
    test_symmetries();
    return 0;
}