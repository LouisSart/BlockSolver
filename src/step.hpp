#pragma once
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

template <typename Block, long unsigned NS>
auto init_root(const Algorithm& scramble, Block& block,
               const std::array<unsigned, NS> rotations) {
    CubieCube scramble_cc(scramble);
    MultiBlockCube<NS> ret;

    for (unsigned k = 0; k < NS; ++k) {
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

    return [](const auto root) {
        return IDAstar(root, apply, estimate, is_solved);
    };
}

template <typename Block1, typename Block2, long unsigned NS>
auto make_split_block_root(const Algorithm& scramble, Block1& block1,
                           Block2& block2,
                           const std::array<unsigned, NS>& rotations) {
    using Cube = std::array<MultiBlockCube<2>, NS>;
    Cube ret;
    CubieCube scramble_cc(scramble);

    for (unsigned k = 0; k < NS; ++k) {
        ret[k][0] = block1.to_coordinate_block_cube(
            scramble_cc.get_conjugate(rotations[k]));
        ret[k][1] = block2.to_coordinate_block_cube(
            scramble_cc.get_conjugate(rotations[k]));
    }

    return make_root(ret);
}

template <typename Block1, typename Block2, long unsigned NS>
auto make_optimal_split_block_solver(
    Block1& block1, Block2& block2, const std::array<unsigned, NS>& rotations) {
    using Cube = std::array<MultiBlockCube<2>, NS>;

    static auto m_table1 = BlockMoveTable(block1);
    static auto m_table2 = BlockMoveTable(block2);
    static auto p_table1 = load_pruning_table(block1);
    static auto p_table2 = load_pruning_table(block2);

    static auto apply = [&rotations](const Move& move, Cube& cube) {
        for (unsigned k = 0; k < NS; ++k) {
            m_table1.sym_apply(move, rotations[k], cube[k][0]);
            m_table2.sym_apply(move, rotations[k], cube[k][1]);
        }
    };

    static auto max_estimate = [](const MultiBlockCube<2>& subcube) {
        auto e1 = p_table1.get_estimate(subcube[0]);
        auto e2 = p_table2.get_estimate(subcube[1]);
        return e1 > e2 ? e1 : e2;
    };

    static auto estimate = [](const Cube& cube) {
        unsigned ret = max_estimate(cube[0]);
        for (unsigned k = 0; k < NS; ++k) {
            unsigned e = max_estimate(cube[k]);
            ret = ret < e ? ret : e;
        }
        return ret;
    };

    static auto is_solved = [&block1, &block2](const Cube& cube) {
        for (unsigned k = 0; k < NS; ++k) {
            if (block1.is_solved(cube[k][0]) && block2.is_solved(cube[k][1]))
                return true;
        }
        return false;
    };

    return [](const auto root) {
        return IDAstar(root, apply, estimate, is_solved);
    };
}