#include "move_table.hpp"

void test_cc_apply_and_move_table_are_equivalent(){
    auto b = Block<1, 1>("OneCornerAndOneEdge", {0}, {0});
    auto bc = BlockCube(b);
    BlockMoveTable<1, 1> table(b);
    CubieCube cc;
    CoordinateBlockCube cbc;

    for (uint move : HTM_Moves){
        table.apply(move, cbc);
        cc.apply(elementary_transformations[move]);
        
        auto cbc_check = bc.to_coordinate_block_cube(cc);
        assert(cbc == cbc_check);
    }
}

void test_222_block_alg_apply(){
    BlockMoveTable<1, 3> table(Block<1, 3>("DLB_222", {7}, {7, 10, 11}));
    CubieCube cc;
    CoordinateBlockCube cbc;
    Algorithm alg({F2, R, U, R3});

    cc.apply(alg);
    table.apply(alg, cbc);
}

int main() {
    test_cc_apply_and_move_table_are_equivalent();
    test_222_block_alg_apply();
    return 0;
}