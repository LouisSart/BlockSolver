#include <type_traits>

#include "algorithm.hpp"
#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "pruning_table.hpp"
#include "search.hpp"

template <bool verbose = true, typename Strategy>
auto solve(const CoordinateBlockCube &cbc,
           const PruningTable<Strategy> &p_table, const Strategy &strat) {
    Node<CoordinateBlockCube> root(cbc, 0);
    return IDAstar<verbose>(root, BlockMoveTable(strat.block), p_table);
}

template <bool verbose = true, typename Strategy>
auto solve(const CoordinateBlockCube &cbc, const Strategy &strat) {
    PruningTable p_table(strat);
    return solve<verbose>(cbc, p_table, strat);
}

template <bool verbose = true, typename Strategy>
auto solve(const Algorithm &scramble, const Strategy &strat) {
    if constexpr (verbose) {
        scramble.show();
    }
    BlockMoveTable m_table(strat.block);
    CoordinateBlockCube cbc;
    m_table.apply(scramble, cbc);
    return solve<verbose>(cbc, strat);
}

template <typename SolutionContainer>
void show(SolutionContainer solutions) {
    for (auto &&s : solutions) {
        s.show();
    }
}