#include "move_table.hpp"

void test_cc_apply_and_move_table_are_equivalent() {
    auto b = Block<1, 1>("OneCornerAndOneEdge", {0}, {0});
    auto bc = BlockCube(b);
    BlockMoveTable<1, 1> table(b);
    CubieCube cc;
    CoordinateBlockCube cbc;

    for (uint move : HTM_Moves) {
        table.apply(move, cbc);
        cc.apply(elementary_transformations[move]);

        auto cbc_check = bc.to_coordinate_block_cube(cc);

        assert(cbc == cbc_check);
    }
}

void test_222_block_alg_apply() {
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

void test_big_move_table() {
    // Move tables used to be stored on the stack, and would sometime
    // exceed stack size
    // New implementation uses std::unique_pointer to make sure the data is
    // stored on the "heap" and garbage collected when the table goes out of
    // scope

    auto table = BlockMoveTable<2, 5>();
}

void test_load() {
    BlockMoveTable<4, 4> table;
    Block<4, 4> b("TopLayer", {0, 1, 2, 3}, {0, 1, 2, 3});
    table.compute_corner_move_tables(b);
    table.compute_edge_move_tables(b);
    table.write();

    BlockMoveTable<4, 4> table_loaded(b);

    for (int i = 0; i < table.cp_table_size; ++i) {
        assert(table.cp_table[i] == table_loaded.cp_table[i]);
    }
}

int main() {
    test_cc_apply_and_move_table_are_equivalent();
    test_222_block_alg_apply();
    test_big_move_table();
    test_load();
    return 0;
}