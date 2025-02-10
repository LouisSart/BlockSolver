#include "step.hpp"
#include "symmetry.hpp"

namespace block_solver_222 {
auto block = Block<1, 3>("DLB_222", {DLB}, {LB, DB, DL});

std::array<unsigned, 8> rotations = {  // 8 different 2x2x2 symmetries
    symmetry_index(0, 0, 0, 0), symmetry_index(0, 1, 0, 0),
    symmetry_index(0, 2, 0, 0), symmetry_index(0, 3, 0, 0),
    symmetry_index(0, 0, 1, 0), symmetry_index(0, 1, 1, 0),
    symmetry_index(0, 2, 1, 0), symmetry_index(0, 3, 1, 0)};

auto solve = make_optimal_block_solver(block, rotations);
auto initialize = make_root_initializer(block, rotations);
}  // namespace block_solver_222
