#include "algorithm.hpp"
#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "search.hpp"

template <bool verbose = true, typename MTable, typename PTable>
auto solve(const CoordinateBlockCube &cbc, const MTable &m_table,
           const PTable &p_table, const unsigned &max_depth = 20) {
    auto solutions = IDAstar(
        Node(cbc, 0),
        [&m_table](const Move &move, CoordinateBlockCube &cube) {
            m_table.apply(move, cube);
        },
        [&p_table](const CoordinateBlockCube &cbc) {
            return p_table.get_estimate(cbc);
        },
        [](const CoordinateBlockCube &cube) { return cube.is_solved(); },
        max_depth);
    return solutions;
}

template <typename Strategy>
Solutions ask_if_generate_and_solve(const Algorithm &scramble,
                                    const Strategy &strat,
                                    const unsigned &max_depth = 20) {
    try {
        auto p_table = strat.load_table();
    } catch (LoadError error) {
        std::cout << "Do you want to generate pruning table ? (y/n) "
                  << std::endl;
        std::string answer;
        std::cin >> answer;
        if (answer == "y") {
            strat.gen_table();
            return {};
        } else {
        };
    }

    auto p_table = strat.load_table();
    auto m_table = BlockMoveTable(strat.block);
    CoordinateBlockCube cbc;
    m_table.apply(scramble, cbc);

    return solve(cbc, m_table, p_table, max_depth);
}