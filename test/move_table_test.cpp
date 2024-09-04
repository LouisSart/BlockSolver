#include "move_table.hpp"

template <unsigned nc, unsigned ne>
void assert_move_table_is_correct(Block<nc, ne> b) {
    b.show();
    BlockMoveTable m_table(b);
    auto cc = CubieCube::random_state();
    auto cbc = b.to_coordinate_block_cube(cc);

    for (Move move : HTM_Moves) {
        m_table.apply(move, cbc);
        cc.apply(move_cc[move]);

        auto cbc_check = b.to_coordinate_block_cube(cc);
        assert(cbc == cbc_check);
    }

    Algorithm scramble(
        "R' U' F  D2 R2 F' L2 D2 F' L  U' B U' D' F2 B2 L2 D F2 U2 D R' U' F");
    auto cbc1 = b.to_coordinate_block_cube(CubieCube(scramble));

    CoordinateBlockCube cbc2;
    m_table.apply(scramble, cbc2);
    assert(cbc1 == cbc2);
}

template <typename Block>
void test_inverse_move_apply(Block&& b) {
    BlockMoveTable table(b);
    auto cc = CubieCube::random_state();
    auto cbc_check = b.to_coordinate_block_cube(cc);
    auto cbc = b.to_coordinate_block_cube(cc);

    for (Move move : HTM_Moves) {
        table.apply(move, cbc);
        table.apply_inverse(move, cbc);

        assert(cbc == cbc_check);
    }
}

void test_222_block_alg_apply() {
    Block<1, 3> b("DLB_222", {DLB}, {LB, DB, DL});
    BlockMoveTable<1, 3> table(b);
    CubieCube cc;
    CoordinateBlockCube cbc;
    Algorithm alg({D2, L, R, F3, U});
    Algorithm no_effect("R U R' F' U2 L' U' L F U2");
    Algorithm Tperm("R U R' U' R' F R2 U' R' U' R U R' F'");

    cc.apply(alg);
    table.apply(alg, cbc);

    assert(b.to_coordinate_block_cube(cc) == cbc);

    cbc.set(0, 0, 0, 0, 0, 0);
    table.apply(Tperm, cbc);
    table.apply(Tperm, cbc);
    assert(cbc.is_solved());

    cbc.set(0, 0, 0, 0, 0, 0);
    table.apply(no_effect, cbc);
    assert(cbc.is_solved());
}

void test_load() {
    BlockMoveTable<4, 4> table;
    Block<4, 4> b("TopLayer", {ULF, URF, URB, ULB}, {UF, UR, UB, UL});
    b.show();
    table.compute_corner_move_tables(b);
    table.compute_edge_move_tables(b);
    table.write(table.block_table_path(b));

    BlockMoveTable<4, 4> table_loaded(b);

    for (int i = 0; i < table.cp_table_size; ++i) {
        assert(table.cp_table[i] == table_loaded.cp_table[i]);
    }
}

void test_eo_table() {
    EOMoveTable table;

    CoordinateBlockCube cube;
    table.apply({F}, cube);
    table.apply({L2, U2, R2, D2, B}, cube);
    assert(cube.is_solved());
    table.apply({F, D, B, U, F}, cube);
    table.apply_inverse({F, D, B, U, F}, cube);
    assert(cube.is_solved());
}

template <typename Block>
void test_sym_apply(Block&& b) {
    BlockMoveTable table(b);

    auto random = CubieCube::random_state();

    for (unsigned s = 0; s < N_SYM; ++s) {
        auto cc_conj = random.get_conjugate(s);
        auto cbc = b.to_coordinate_block_cube(cc_conj);

        for (Move move : HTM_Moves) {
            table.sym_apply(move, s, cbc);
            cc_conj.apply(move);
        }
        assert(cbc == b.to_coordinate_block_cube(cc_conj));
    }
}

int main() {
    assert_move_table_is_correct(Block<8, 0>(
        "AllCorners", {ULF, URF, URB, ULB, DLF, DRF, DRB, DLB}, {}));
    assert_move_table_is_correct(
        Block<0, 4>("BottomCross", {}, {DF, DR, DB, DL}));
    assert_move_table_is_correct(
        Block<1, 1>("OneCornerAndOneEdge", {ULF}, {UF}));
    assert_move_table_is_correct(
        Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL}));
    test_222_block_alg_apply();
    test_inverse_move_apply(
        Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL}));
    test_load();
    test_eo_table();
    test_sym_apply(Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL}));
    return 0;
}