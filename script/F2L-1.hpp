#include <iostream>

#include "option.hpp"
#include "step.hpp"

namespace block_solver_F2Lm1 {
constexpr unsigned NB = 2;   // the F2L-1 is splitted into 2 blocks
constexpr unsigned NS = 24;  // number of F2L-1 symmetries
auto block1 = Block<3, 3>("222_w_extra_corners", {DLF, DLB, DRB}, {DL, LB, DB});
auto block2 = Block<2, 4>("2_squares", {DLF, DRB}, {LF, DF, RB, DR});

std::array<unsigned, NS> rotations{
    symmetry_index(0, 0, 0, 0), symmetry_index(0, 1, 0, 0),
    symmetry_index(0, 2, 0, 0), symmetry_index(0, 3, 0, 0),
    symmetry_index(0, 0, 1, 0), symmetry_index(0, 1, 1, 0),
    symmetry_index(0, 2, 1, 0), symmetry_index(0, 3, 1, 0),
    symmetry_index(1, 0, 0, 0), symmetry_index(1, 1, 0, 0),
    symmetry_index(1, 2, 0, 0), symmetry_index(1, 3, 0, 0),
    symmetry_index(1, 0, 1, 0), symmetry_index(1, 1, 1, 0),
    symmetry_index(1, 2, 1, 0), symmetry_index(1, 3, 1, 0),
    symmetry_index(2, 0, 0, 0), symmetry_index(2, 1, 0, 0),
    symmetry_index(2, 2, 0, 0), symmetry_index(2, 3, 0, 0),
    symmetry_index(2, 0, 1, 0), symmetry_index(2, 1, 1, 0),
    symmetry_index(2, 2, 1, 0), symmetry_index(2, 3, 1, 0),
};

auto solve_F2Lm1 = make_optimal_split_block_solver(block1, block2, rotations);
}  // namespace block_solver_F2Lm1