#include <type_traits>

#include "algorithm.hpp"
#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "pruning_table.hpp"
#include "search.hpp"

template <bool verbose = true, typename Strategy>
auto solve(const CoordinateBlockCube &cbc,
           const PruningTable<Strategy> &p_table, const Strategy &strat,
           const unsigned &max_depth = 20) {
    Node<CoordinateBlockCube> root(cbc, 0);
    return IDAstar<verbose>(root, BlockMoveTable(strat.block), p_table,
                            max_depth);
}

template <bool verbose = true, typename Strategy>
auto solve(const CoordinateBlockCube &cbc, const Strategy &strat,
           const unsigned &max_depth = 20) {
    auto p_table = strat.load_table();
    return solve<verbose>(cbc, p_table, strat, max_depth);
}

template <bool verbose = true, typename Strategy>
auto solve(const Algorithm &scramble, const PruningTable<Strategy> &p_table,
           const Strategy &strat, const unsigned &max_depth = 20) {
    BlockMoveTable m_table(strat.block);
    CoordinateBlockCube cbc;
    m_table.apply(scramble, cbc);
    return solve<verbose>(cbc, p_table, strat, max_depth);
}

template <bool verbose = true, typename Strategy>
auto solve(const Algorithm &scramble, const Strategy &strat,
           const unsigned &max_depth = 20) {
    if constexpr (verbose) {
        scramble.show();
    }
    BlockMoveTable m_table(strat.block);
    CoordinateBlockCube cbc;
    m_table.apply(scramble, cbc);
    return solve<verbose>(cbc, strat, max_depth);
}

template <typename SolutionContainer>
void show(SolutionContainer solutions) {
    for (auto &&s : solutions) {
        s.show();
    }
}