#pragma once
#include "move_table.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"
#include "symmetry.hpp"

namespace block_solver_223 {
// The optimal pruning table for the 2x2x3 is
// too big, so we use 1x2x3s instead. Solving the DB 2x2x3
// is just the same as solving the 2 1x2x3 that share the DB edge

constexpr unsigned NS = 12;  // number of 2x2x3 symmetries
using Cube = MultiBlockCube<NS>;

Block<2, 5> block("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});

std::array<unsigned, NS> rotations = {
    symmetry_index(0, 0, 0, 0),  // DL 0
    symmetry_index(0, 3, 0, 0),  // DB 1
    symmetry_index(0, 2, 0, 0),  // DR 2
    symmetry_index(0, 1, 0, 0),  // DF 3
    symmetry_index(0, 1, 1, 0),  // UF 4
    symmetry_index(0, 2, 1, 0),  // UL 5
    symmetry_index(0, 3, 1, 0),  // UB 6
    symmetry_index(0, 0, 1, 0),  // UR 7
    symmetry_index(2, 0, 0, 0),  // LB 8
    symmetry_index(2, 1, 0, 0),  // LF 9
    symmetry_index(2, 3, 0, 0),  // RB 10
    symmetry_index(2, 2, 0, 0)   // RF 11
};

auto initialize = make_root_initializer(block, rotations);
auto cc_initialize = make_root_cc_initializer(block, rotations);
auto mt = BlockMoveTable(block);
auto pt = load_pruning_table(block);
auto apply = get_sym_apply<NS>(mt, rotations);
auto estimate = get_estimator<NS>(pt, block.get_indexer());
auto is_solved = get_is_solved<NS>(block);

auto solve = [](const auto root, const unsigned max_depth = 20,
                const unsigned slackness = 0) {
    return IDAstar<false>(root, apply, estimate, is_solved, max_depth,
                          slackness);
};

}  // namespace block_solver_223