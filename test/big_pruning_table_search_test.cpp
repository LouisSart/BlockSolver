#include "pruning_table.hpp"
#include "search.hpp"

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
    auto DL_223 = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});

    std::cout << "2x2x3 Solve with optimal pruning table" << std::endl;
    search<OptimalPruningTable<2, 5>>(DL_223, 10);

    std::cout << "2x2x3 Solve with permutation pruning table" << std::endl;
    search<PermutationPruningTable<2, 5>>(DL_223, 10);
    return 0.;
}