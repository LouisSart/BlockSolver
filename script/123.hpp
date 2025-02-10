#include "move_table.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"
#include "symmetry.hpp"

namespace block_solver_123 {
auto block = Block<2, 3>("DB_123", {DLB, DRB}, {DB, RB, LB});

std::array<unsigned, 24> rotations = {  // 24 different 1x2x3 symmetries
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
    symmetry_index(2, 2, 1, 0), symmetry_index(2, 3, 1, 0)};

auto solve = make_optimal_block_solver(block, rotations);
}  // namespace block_solver_123