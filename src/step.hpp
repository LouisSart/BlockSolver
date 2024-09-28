#include <tuple>  // tables stored as tuples in Mover and Pruner

#include "coordinate_block_cube.hpp"  // MultiBlockCube
#include "move_table.hpp"             // BlockMoveTable
#include "pruning_table.hpp"          // load_ptr(Strategy)
#include "search.hpp"                 // DFS and IDA*

template <std::size_t NB, typename MoveTable>
auto get_sym_apply(const MoveTable& m_table,
                   const std::array<unsigned, NB>& rotations) {
    return [&m_table, &rotations](const Move& move, MultiBlockCube<NB>& cube) {
        for (unsigned k = 0; k < NB; ++k) {
            m_table.sym_apply(move, rotations[k], cube[k]);
        }
    };
}

template <std::size_t NB, typename Block>
auto get_is_solved(Block& block) {
    return [&block](const MultiBlockCube<NB>& cube) {
        // Returns true if at least one of the cbc is solved
        for (auto cbc : cube) {
            if (block.is_solved(cbc)) return true;
        }
        return false;
    };
}

template <std::size_t NB, typename PruningTable>
auto get_estimator(const PruningTable& p_table) {
    return [&p_table](const MultiBlockCube<NB>& cube) {
        // Return the minimum estimate over all the cbcs
        unsigned ret = p_table.get_estimate(cube[0]);
        for (auto cbc : cube) {
            auto e = p_table.get_estimate(cbc);
            ret = (e < ret) ? e : ret;
        }
        return ret;
    };
}

template <typename Block, std::size_t NB>
auto init_root(const Algorithm& scramble, Block& block,
               const std::array<unsigned, NB> rotations) {
    CubieCube scramble_cc(scramble);
    MultiBlockCube<NB> ret;

    for (unsigned k = 0; k < NB; ++k) {
        ret[k] = block.to_coordinate_block_cube(
            scramble_cc.get_conjugate(rotations[k]));
    }
    return make_root(ret);
}

template <unsigned nc, unsigned ne, long unsigned NS>
auto make_optimal_block_solver(Block<nc, ne>& block,
                               const std::array<unsigned, NS>& rotations) {
    static auto m_table = BlockMoveTable(block);
    static auto p_table = load_pruning_table(block);
    static auto apply = get_sym_apply<NS>(m_table, rotations);
    static auto estimate = get_estimator<NS>(p_table);
    static auto is_solved = get_is_solved<NS>(block);

    return [](const auto node) {
        return IDAstar(node, apply, estimate, is_solved);
    };
}