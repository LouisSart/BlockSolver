#pragma once
#include <tuple>  // tables stored as tuples in Mover and Pruner

#include "coordinate_block_cube.hpp"  // MultiBlockCube
#include "move_table.hpp"             // BlockMoveTable
#include "pruning_table.hpp"          // load_ptr(Strategy)
#include "search.hpp"                 // DFS and IDA*

template <unsigned nc, unsigned ne>
auto load_pruning_table(Block<nc, ne>& b) {
    // Load the pruning table for the given block
    constexpr size_t table_size = b.n_es * b.n_cs;
    PruningTable<table_size> ptable;
    bool check = ptable.load(b.id);
    if (!check) {
        print("Generating pruning table", b.id);
        BlockMoveTable<nc, ne> mtable(b);
        auto root = b.to_coordinate_block_cube(CubieCube());
        ptable.template generate<true>(root, mtable.get_apply(),
                                       b.get_indexer(), b.get_from_index(),
                                       HTM_Moves);
    }
    ptable.write(b.id);
    return ptable;
};

template <std::size_t NS, typename MoveTable>
auto get_sym_apply(const MoveTable& m_table,
                   const std::array<unsigned, NS>& rotations) {
    return [&m_table, &rotations](const Move& move, MultiBlockCube<NS>& cube) {
        for (unsigned k = 0; k < NS; ++k) {
            m_table.sym_apply(move, rotations[k], cube[k]);
        }
    };
}

template <std::size_t NS, typename Block>
auto get_is_solved(Block& block) {
    return [&block](const MultiBlockCube<NS>& cube) {
        // Returns true if at least one of the symmetries is solved
        for (auto cbc : cube) {
            if (block.is_solved(cbc)) return true;
        }
        return false;
    };
}

template <std::size_t NB, typename PruningTable, typename Indexer>
auto get_estimator(const PruningTable& p_table, const Indexer& index) {
    return [&p_table, &index](const MultiBlockCube<NB>& cube) {
        // Return the minimum estimate over all the cbcs
        unsigned ret = p_table.estimate(index(cube[0]));
        for (auto cbc : cube) {
            auto e = p_table.estimate(index(cbc));
            ret = (e < ret) ? e : ret;
        }
        return ret;
    };
}

template <typename Block, long unsigned NS>
auto init_root(const CubieCube& scramble_cc, Block& block,
               const std::array<unsigned, NS>& rotations) {
    MultiBlockCube<NS> ret;

    for (unsigned k = 0; k < NS; ++k) {
        ret[k] = block.to_coordinate_block_cube(
            scramble_cc.get_conjugate(rotations[k]));
    }
    return make_root(ret);
}

template <typename Block, long unsigned NS>
auto make_root_initializer(Block& block,
                           const std::array<unsigned, NS>& rotations) {
    return [&block, &rotations](const Algorithm& scramble) {
        CubieCube scramble_cc(scramble);
        return init_root(scramble_cc, block, rotations);
    };
}
template <typename Block, long unsigned NS>
auto make_root_cc_initializer(Block& block,
                              const std::array<unsigned, NS>& rotations) {
    return [&block, &rotations](const CubieCube& scramble_cc) {
        return init_root(scramble_cc, block, rotations);
    };
}

template <unsigned nc, unsigned ne, long unsigned NS>
auto make_optimal_block_solver(Block<nc, ne>& block,
                               const std::array<unsigned, NS>& rotations) {
    static auto m_table = BlockMoveTable(block);
    static auto p_table = load_pruning_table(block);
    static auto apply = get_sym_apply<NS>(m_table, rotations);
    static auto estimate = get_estimator<NS>(p_table, block.get_indexer());
    static auto is_solved = get_is_solved<NS>(block);

    return [](const auto root, const unsigned max_depth = 20,
              const unsigned slackness = 0) {
        return IDAstar<false>(root, apply, estimate, is_solved, max_depth,
                              slackness);
    };
}

template <typename Block1, typename Block2, long unsigned NS>
auto make_split_block_root(const CubieCube& scramble_cc, Block1& block1,
                           Block2& block2,
                           const std::array<unsigned, NS>& rotations) {
    using Cube = std::array<MultiBlockCube<2>, NS>;
    Cube ret;

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

    static auto max_estimate = [block1,
                                block2](const MultiBlockCube<2>& subcube) {
        auto e1 = p_table1.estimate(block1.index(subcube[0]));
        auto e2 = p_table2.estimate(block2.index(subcube[1]));
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

    return [](const auto root, const unsigned max_depth = 20,
              const unsigned slackness = 0) {
        return IDAstar<false>(root, apply, estimate, is_solved, max_depth,
                              slackness);
    };
}