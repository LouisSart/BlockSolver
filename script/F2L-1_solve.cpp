#include <iostream>

#include "block_cube.hpp"
#include "pruning_table.hpp"
#include "search.hpp"

auto LF_column = Block<2, 1>("LF_column", {ULF, DLB}, {LF});
auto DLB_222 = Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB});
auto DL_223 = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});
auto DLB_F2Lm1 =
    Block<3, 7>("DLB_F2L-1", {DLF, DLB, DRB}, {LF, LB, DF, DB, DL, RB, DR});

template <typename PTable, unsigned nc, unsigned ne>
auto search(const Block<nc, ne>& b, unsigned expected_number_of_solutions) {
    PTable p_table(b);
    BlockMoveTable m_table(b);
    CoordinateBlockCube cbc;
    Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                        U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
    scramble.show();
    m_table.apply(scramble, cbc);

    Node<CoordinateBlockCube> root(cbc, 0);
    auto solutions = IDAstar(root, m_table, p_table);
    for (auto&& s : solutions) {
        s.show();
    }
    assert(solutions.size() == expected_number_of_solutions);
}

int main() {
    auto b = DLB_F2Lm1;
    PermutationPruningTable table(b);
    // table.gen<true>();
    search<PermutationPruningTable<3, 7>>(b, 1);
}