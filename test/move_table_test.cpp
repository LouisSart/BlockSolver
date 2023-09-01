#include "move_table.hpp"

void test_cc_apply_and_move_table_are_equivalent(){
    BlockMoveTable<1, 1> table(Block<1, 1>("OneCornerAndOneEdge", {0}, {0}));
    CubieCube cc;
    CoordinateBlockCube cbc;

    for (uint move : HTM_Moves){
        table.apply(move, cbc);
        cc.apply(elementary_transformations[move]);
    }
    auto cbc_check = table.bc.to_coordinate_block_cube(cc);
    cbc.show();
    cbc_check.show();
    assert(cbc == cbc_check);
}

void test_222_block_alg_apply(){
    BlockMoveTable<1, 3> table(Block<1, 3>("DLB_222", {7}, {7, 10, 11}));

}

int main() {
    test_cc_apply_and_move_table_are_equivalent();
    return 0;
}