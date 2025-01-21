#include "move_table.hpp"

template <unsigned nc, unsigned ne>
void test_move_table_apply(Block<nc, ne>&& b) {
    BlockMoveTable m_table(b);
    const auto cc_start = CubieCube::random_state();
    const auto cbc_start = b.to_coordinate_block_cube(cc_start);

    CubieCube cc;
    CoordinateBlockCube cbc;

    for (Move move : HTM_Moves) {
        cc = cc_start;
        cbc = cbc_start;
        m_table.apply(move, cbc);
        cc.apply(move);

        assert(cbc == b.to_coordinate_block_cube(cc));
    }
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
    CoordinateBlockCube cbc = b.to_coordinate_block_cube(cc);

    Algorithm alg({D, L, R, F3, U});
    Algorithm no_effect("R U R' F' U2 L' U' L F U2");
    Algorithm Tperm("R U R' U' R' F R2 U' R' U' R U R' F'");

    for (auto a : {alg, Tperm, no_effect}) {
        table.apply(a, cbc);
        cc.apply(a);

        assert(cbc == b.to_coordinate_block_cube(cc));
    }

    cbc.set(0, 0, 0, 0, 0, 0);
    table.apply(Tperm, cbc);
    table.apply(Tperm, cbc);
    assert(b.is_solved(cbc));

    cbc.set(0, 0, 0, 0, 0, 0);
    table.apply(no_effect, cbc);
    assert(b.is_solved(cbc));
}

void test_load() {
    BlockMoveTable<4, 4> table;
    Block<4, 4> b("TopLayer", {ULF, URF, URB, ULB}, {UF, UR, UB, UL});
    table.compute_corner_move_tables(b);
    table.compute_edge_move_tables(b);
    table.write(table.block_table_path(b));

    BlockMoveTable<4, 4> table_loaded(b);

    for (int i = 0; i < table.cp_table_size; ++i) {
        assert(table.cp_table[i] == table_loaded.cp_table[i]);
    }
    for (int i = 0; i < table.co_table_size; ++i) {
        assert(table.co_table[i] == table_loaded.co_table[i]);
    }
    for (int i = 0; i < table.ep_table_size; ++i) {
        assert(table.ep_table[i] == table_loaded.ep_table[i]);
    }
    for (int i = 0; i < table.eo_table_size; ++i) {
        assert(table.eo_table[i] == table_loaded.eo_table[i]);
    }
}

void test_eo_table() {
    EOMoveTable computed, loaded;
    computed.compute_table();
    computed.write();
    loaded.load();

    for (unsigned c = 0; c < ipow(2, NE - 1); ++c) {
        for (Move m : HTM_Moves) {
            assert(computed.table[c * N_HTM_MOVES + m] ==
                   loaded.table[c * N_HTM_MOVES + m]);
        }
    }

    EOMoveTable m_table;
    const auto cc_start = CubieCube::random_state();
    const unsigned eo_c = eo_index<NE, true>(cc_start.eo);

    CubieCube cc;
    CoordinateBlockCube cbc;

    for (Move move : HTM_Moves) {
        cc = cc_start;
        cbc.ceo = eo_c;
        m_table.apply(move, cbc);
        cc.apply(move);

        assert((cbc.ceo == eo_index<NE, true>(cc.eo)));
    }
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

void test_to_cubie_cube() {
    Block<2, 5> b("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});
    BlockMoveTable m_table(b);

    auto cc = CubieCube::random_state();
    CoordinateBlockCube cbc = b.to_coordinate_block_cube(cc);
    CubieCube cc_copy;
    CoordinateBlockCube cbc_copy;

    for (Move m : HTM_Moves) {
        cc_copy = cc;
        cbc_copy = cbc;

        cc_copy.apply(m);
        m_table.apply(m, cbc_copy);

        b.to_cubie_cube(cbc_copy).show();
        cc_copy.show();
        assert(cbc_copy == b.to_coordinate_block_cube(cc_copy));
    }
}

int main() {
    test_move_table_apply(Block<8, 0>(
        "AllCorners", {ULF, URF, URB, ULB, DLF, DRF, DRB, DLB}, {}));
    test_move_table_apply(Block<0, 4>("BottomCross", {}, {DF, DR, DB, DL}));
    test_move_table_apply(Block<1, 1>("OneCornerAndOneEdge", {ULF}, {UF}));
    test_move_table_apply(
        Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL}));
    test_222_block_alg_apply();
    test_inverse_move_apply(
        Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL}));
    test_load();
    test_eo_table();
    test_sym_apply(Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL}));
    return 0;
}