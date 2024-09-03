#include <cassert>

#include "algorithm.hpp"
#include "block_cube.hpp"
#include "coordinate_block_cube.hpp"
#include "cubie_cube.hpp"

void test_block() {
    Block<2, 5> b("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});
    b.show();
}

template <unsigned nc, unsigned ne>
void test_to_cbc_from_cc_and_back(Block<nc, ne> b) {
    BlockCube bc(b);
    auto cc = CubieCube::random_state();
    auto cbc = bc.to_coordinate_block_cube(cc);
    assert(cbc == bc.to_coordinate_block_cube(bc.to_cubie_cube(cbc)));
}

void test_corner_permutation_table() {
    BlockCube<1, 0> bc("UFL corner", {ULF}, {});
    CubieCube cc, cc_copy;
    unsigned num_layout = 8;
    unsigned num_perm = 1;
    CoordinateBlockCube cbc;
    for (unsigned ccl = 0; ccl < num_layout; ++ccl) {
        for (unsigned ccp = 0; ccp < num_perm; ++ccp) {
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
