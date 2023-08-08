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

int main() {
    test_to_cbc_from_cc_and_back();
    return 0;
}
