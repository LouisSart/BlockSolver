#pragma once
#include <iostream>

#include "223.hpp"
#include "option.hpp"
#include "step.hpp"

namespace b223 = block_solver_223;

namespace block_solver_F2Lm1 {
constexpr unsigned NB = 2;   // the F2L-1 is splitted into 2 2x2x3 blocks
constexpr unsigned NS = 24;  // number of F2L-1 symmetries
auto block = b223::block;
using Cube = b223::Cube;

// Array of the 2x2x3 block pairs which need to be
// simultaneously solved to have F2L-1 solved
std::array<std::array<unsigned, NB>, NS> block_pairs = {
    {{0, 1}, {1, 2}, {2, 3},  {3, 0},  {4, 5},  {5, 6},  {6, 7},  {7, 4},
     {8, 0}, {0, 9}, {9, 5},  {5, 8},  {2, 10}, {10, 7}, {7, 11}, {11, 2},
     {3, 9}, {9, 4}, {4, 11}, {11, 3}, {1, 8},  {8, 6},  {6, 10}, {10, 1}}};

unsigned local_estimate(const CoordinateBlockCube& cbc1,
                        const CoordinateBlockCube& cbc2) {
    return std::max(b223::pt.estimate(block.index(cbc1)),
                    b223::pt.estimate(block.index(cbc2)));
}

auto estimate(const Cube& cube) {
    unsigned ret = 1000;
    for (auto&& [k, l] : block_pairs) {
        ret = std::min(ret, local_estimate(cube[k], cube[l]));
    }
    return ret;
}

bool local_is_solved(const CoordinateBlockCube& cbc1,
                     const CoordinateBlockCube& cbc2) {
    return block.is_solved(cbc1) && block.is_solved(cbc2);
}

bool is_solved(const Cube& cube) {
    for (auto&& [k, l] : block_pairs) {
        if (local_is_solved(cube[k], cube[l])) return true;
    }
    return false;
}

auto initialize = b223::initialize;
auto cc_initialize = b223::cc_initialize;

auto solve = [](const auto root, const unsigned max_depth = 20,
                const unsigned slackness = 0) {
    return IDAstar<false>(root, b223::apply, estimate, is_solved, max_depth,
                          slackness);
};
}  // namespace block_solver_F2Lm1