#include <cassert>

#include "algorithm.hpp"
#include "block_cube.hpp"
#include "coordinate_block_cube.hpp"
#include "cubie_cube.hpp"

void test_block() {
    Block<2, 5> b("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});
    b.show();
    for (auto &c : b.c_order) {
        assert((ULF <= c) && (c <= DLB));
    }
    for (auto &e : b.e_order) {
        assert((UF <= e) && (e <= DL));
    }
    auto [cb, eb] = b.split_corners_and_edges();
    cb.show();
    eb.show();
}

template <unsigned nc, unsigned ne>
void test_to_cbc_from_cc_and_back(Block<nc, ne> b) {
    BlockCube bc(b);
    CubieCube cc, return_cc;
    CoordinateBlockCube cbc;
    Algorithm alg({D2, L, R, F3, U});

    cc.apply(alg);
    cbc = bc.to_coordinate_block_cube(cc);
    return_cc = bc.to_cubie_cube(cbc);

    // Assert cc and return_cc have the block
    // pieces in the same spots and orientation
    if (nc > 0) {
        for (int i = 0; i < 8; ++i) {
            if (return_cc.cp[i] < 8) {
                assert(return_cc.cp[i] == cc.cp[i]);
            }

            if (return_cc.co[i] < 3) {
                assert(return_cc.co[i] == cc.co[i]);
            }
        }
    }

    if (ne > 0) {
        for (int i = 0; i < 12; ++i) {
            if (return_cc.ep[i] < 12) {
                assert(return_cc.ep[i] == cc.ep[i]);
            }

            if (return_cc.eo[i] < 2) {
                assert(return_cc.eo[i] == cc.eo[i]);
            }
        }
    }
}

void test_corner_permutation_table() {
    BlockCube<1, 0> bc("UFL corner", {ULF}, {});
    CubieCube cc, cc_copy;
    uint num_layout = 8;
    uint num_perm = 1;
    CoordinateBlockCube cbc;
    for (uint ccl = 0; ccl < num_layout; ++ccl) {
        for (uint ccp = 0; ccp < num_perm; ++ccp) {
            cbc.ccl = ccl;
            cbc.ccp = ccp;
            cc = bc.to_cubie_cube(cbc);
            std::cout << "Permutation coordinate " << ccl * num_perm + ccp
                      << ": ";
            for (auto move_idx : HTM_Moves) {
                auto move = move_cc[move_idx];
                cc_copy = cc;
                assert(&cc_copy != &cc);
                assert(cc_copy == cc);
                cc_copy.apply(move);
                assert(cc_copy != cc);
                cbc = bc.to_coordinate_block_cube(cc_copy);
                std::cout << cbc.ccl * num_perm + cbc.ccp << " ";
            }
            std::cout << std::endl;
        }
    }
}

int main() {
    test_block();
    test_to_cbc_from_cc_and_back(
        Block<1, 1>("OneCornerAndOneEdge", {ULF}, {UF}));
    test_to_cbc_from_cc_and_back(Block<8, 0>(
        "AllCorners", {ULF, URF, URB, ULB, DLF, DRF, DRB, DLB}, {}));
    test_to_cbc_from_cc_and_back(
        Block<0, 4>("BottomCross", {}, {DF, DR, DB, DL}));
    test_corner_permutation_table();
    return 0;
}
