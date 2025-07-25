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

constexpr unsigned NB = 2;   // the 2x2x3 is splitted into 2 1x2x3 blocks
constexpr unsigned NS = 12;  // number of 2x2x3 symmetries
using Cube = std::array<MultiBlockCube<NB>, NS>;

auto block = Block<2, 3>("DB_123", {DLB, DRB}, {DB, RB, LB});

std::array<std::array<unsigned, NB>, NS> rotations = {{
    {symmetry_index(0, 0, 0, 0), symmetry_index(2, 3, 0, 0)},  // DB
    {symmetry_index(0, 1, 0, 0), symmetry_index(2, 0, 0, 0)},  // DL
    {symmetry_index(0, 2, 0, 0), symmetry_index(2, 1, 0, 0)},  // DF
    {symmetry_index(0, 3, 0, 0), symmetry_index(2, 2, 0, 0)},  // DR
    {symmetry_index(0, 0, 1, 0), symmetry_index(2, 3, 1, 0)},  // UB
    {symmetry_index(0, 1, 1, 0), symmetry_index(2, 0, 1, 0)},  // UR
    {symmetry_index(0, 3, 1, 0), symmetry_index(2, 2, 1, 0)},  // UL
    {symmetry_index(0, 2, 1, 0), symmetry_index(2, 1, 1, 0)},  // UF
    {symmetry_index(1, 0, 0, 0), symmetry_index(1, 3, 1, 0)},  // LB
    {symmetry_index(1, 1, 0, 0), symmetry_index(1, 2, 1, 0)},  // LF
    {symmetry_index(1, 3, 0, 0), symmetry_index(1, 0, 1, 0)},  // RB
    {symmetry_index(1, 2, 0, 0), symmetry_index(1, 1, 1, 0)},  // RF
}};

auto m_table = BlockMoveTable(block);
auto p_table = load_pruning_table(block);

void local_apply(const Move& move, const std::array<unsigned, NB>& syms,
                 MultiBlockCube<NB>& subcube) {
    m_table.sym_apply(move, syms[0], subcube[0]);
    m_table.sym_apply(move, syms[1], subcube[1]);
};

void apply(const Move& move, Cube& cube) {
    for (unsigned k = 0; k < NS; ++k) {
        local_apply(move, rotations[k], cube[k]);
    }
};

unsigned get_estimate(const CoordinateBlockCube& subcube) {
    return p_table.estimate(block.index(subcube));
};

unsigned max_estimate(const MultiBlockCube<NB>& cube) {
    return get_estimate(cube[0]) > get_estimate(cube[1])
               ? get_estimate(cube[0])
               : get_estimate(cube[1]);
}

auto estimate = [](const Cube& cube) {
    unsigned ret = max_estimate(cube[0]);
    for (unsigned k = 0; k < NS; ++k) {
        unsigned e = max_estimate(cube[k]);
        ret = ret < e ? ret : e;
    }
    return ret;
};

bool local_is_solved(const MultiBlockCube<NB>& subcube) {
    return (block.is_solved(subcube[0]) && block.is_solved(subcube[1]));
}

auto is_solved = [](const Cube& cube) {
    for (unsigned k = 0; k < NS; ++k) {
        if (local_is_solved(cube[k])) return true;
    }
    return false;
};

auto cc_initialize(const CubieCube& scramble_cc) {
    Cube ret;

    for (unsigned k = 0; k < NS; ++k) {
        ret[k][0] = block.to_coordinate_block_cube(
            scramble_cc.get_conjugate(rotations[k][0]));
        ret[k][1] = block.to_coordinate_block_cube(
            scramble_cc.get_conjugate(rotations[k][1]));
    }

    return make_root(ret);
}

auto initialize(const Algorithm& scramble) {
    CubieCube scramble_cc(scramble);
    return cc_initialize(scramble_cc);
}

using NodePtr = Node<Cube>::sptr;

auto solve(const NodePtr root, const unsigned move_budget = 20,
           const unsigned slackness = 0) {
    return IDAstar<false>(root, apply, estimate, is_solved, move_budget,
                          slackness);
}

}  // namespace block_solver_223