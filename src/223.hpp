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
    symmetry_index(0, 0, 0, 0),  // DL
    symmetry_index(0, 1, 0, 0),  // DB
    symmetry_index(0, 2, 0, 0),  // DR
    symmetry_index(0, 3, 0, 0),  // DF
    symmetry_index(0, 0, 1, 0),  // UR
    symmetry_index(0, 1, 1, 0),  // UF
    symmetry_index(0, 2, 1, 0),  // UL
    symmetry_index(0, 3, 1, 0),  // UB
    symmetry_index(1, 0, 0, 0),  // LB
    symmetry_index(1, 2, 0, 0),  // LF
    symmetry_index(1, 0, 0, 1),  // RB
    symmetry_index(1, 2, 0, 1)   // RF
};

auto solve = make_optimal_block_solver(block, rotations);
auto initialize = make_root_initializer(block, rotations);
auto cc_initialize = make_root_cc_initializer(block, rotations);

}  // namespace block_solver_223