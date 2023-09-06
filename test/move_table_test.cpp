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
    BlockCube<1, 3> bc("DLB_222", {7}, {7, 10, 11});
    BlockMoveTable<1, 3> table(bc.b);
    CubieCube cc;
    CoordinateBlockCube cbc;
    Algorithm alg({D2, L, R, F3, U});
    Algorithm no_effect({R, U, R3, F3, U2, L3, U3, L, F, U2});
    Algorithm Tperm({R, U, R3, U3, R3, F, R2, U3, R3, U3, R, U, R3, F3});

    cc.apply(alg);
    table.apply(alg, cbc);

    assert(bc.to_coordinate_block_cube(cc) == cbc);

    cbc.set(0, 0, 0, 0, 0, 0);
    table.apply(Tperm, cbc);
    table.apply(Tperm, cbc);
    assert(cbc.is_solved());

    cbc.set(0, 0, 0, 0, 0, 0);
    table.apply(no_effect, cbc);
    assert(cbc.is_solved());
}

int main() {
    test_cc_apply_and_move_table_are_equivalent();
    test_222_block_alg_apply();
    return 0;
}