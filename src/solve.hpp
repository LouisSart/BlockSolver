#include <type_traits>

#include "algorithm.hpp"
#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "pruning_table.hpp"
#include "search.hpp"

template <bool verbose = true, typename PTable, typename Strategy>
auto solve(const CoordinateBlockCube &cbc, const PTable &p_table,
           const Strategy &strat, const unsigned &max_depth = 20) {
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

template <bool verbose = true, typename PTable, typename Strategy>
auto solve(const Algorithm &scramble, const PTable &p_table,
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

template <typename Strategy>
Solutions ask_if_generate_and_solve(const Algorithm &scramble,
                                    const Strategy &strat,
                                    const unsigned &max_depth = 20) {
    try {
        auto solutions = solve(scramble, strat);
        return solutions;
    } catch (LoadError error) {
        std::cout << "Do you want to generate pruning table ? (y/n) "
                  << std::endl;
        std::string answer;
        std::cin >> answer;
        if (answer == "y") {
            strat.gen_table();
            return solve(scramble, strat);
        } else
            return {};
    }
}