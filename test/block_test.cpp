#include "algorithm.hpp"
#include "coordinate_block_cube.hpp"
#include "block_cube.hpp"
#include "cubie_cube.hpp"

#include <cassert>

void test_to_cbc_from_cc_and_back(){
    Block<1, 3> b("DLB_222", {7}, {7, 10, 11});
    BlockCube bc(b);
    CubieCube cc;
    Algorithm alg({D2, L, R, F3, U});
    alg.apply(cc);
    bc.from_cubie_cube(cc);
    CoordinateBlockCube cbc(bc);

    bc.from_coordinate_block_cube(cbc);
    CubieCube return_cc;
    bc.to_cubie_cube(return_cc);

    // Assert cc and return_cc have the block
    // pieces in the same spots and orientation
    for (int i=0; i<8; ++i)
    {
        if (return_cc.cp[i] < 8) {
            assert(return_cc.cp[i] == cc.cp[i]);
        }

        if (return_cc.co[i] < 3) {
            assert(return_cc.co[i] == cc.co[i]);
        }
    }

    for (int i=0; i<12; ++i)
    {
        if (return_cc.ep[i] < 12) {
            assert(return_cc.ep[i] == cc.ep[i]);
        }

        if (return_cc.eo[i] < 2) {
            assert(return_cc.eo[i] == cc.eo[i]);
        }
    }
}

void test_corner_permutation_table() {
    Block<1, 1> b("UFL corner", {0}, {0});
    BlockCube bc(b);
    CubieCube cc;
    uint num_layout = 8;
    uint num_perm = 1;
    CoordinateBlockCube cbc;

    for (uint cl=0; cl<num_layout; ++cl) {
        for (uint cp=0; cp<num_perm; ++cp) {
            cbc.ccl = cl;
            cbc.ccp = cp;
            bc.from_coordinate_block_cube(cbc);
            bc.to_cubie_cube(cc);
            CubieCube cc_copy;
            std::cout << "Permutation coordinate " << cl * num_perm + cp << ": ";
            for (auto move_idx : HTM_Moves) {
                auto move = elementary_transformations[move_idx];
                cc_copy = cc;
                cc_copy.apply(move);
                bc.from_cubie_cube(cc_copy);
                cbc.from_block_cube(bc);
                std::cout << cbc.ccl * num_perm + cbc.ccp << " ";
            }
            std::cout << std::endl;
        }
    }
}

int main() {
    test_to_cbc_from_cc_and_back();
    test_corner_permutation_table();
    return 0;
}
