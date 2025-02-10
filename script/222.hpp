#include "move_table.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"
#include "symmetry.hpp"
#include "utils.hpp"

namespace block_solver_222 {
auto block = Block<1, 3>("DLB_222", {DLB}, {LB, DB, DL});

std::array<unsigned, 8> rotations = {  // 8 different 2x2x2 symmetries
    symmetry_index(0, 0, 0, 0), symmetry_index(0, 1, 0, 0),
    symmetry_index(0, 2, 0, 0), symmetry_index(0, 3, 0, 0),
    symmetry_index(0, 0, 1, 0), symmetry_index(0, 1, 1, 0),
    symmetry_index(0, 2, 1, 0), symmetry_index(0, 3, 1, 0)};

auto solve_222 = make_optimal_block_solver(block, rotations);
}  // namespace block_solver_222

// int main(int argc, const char *argv[]) {
//     auto scramble = Algorithm(argv[argc - 1]);
//     scramble.show();

//     auto root = init_root(scramble, block, rotations);

//     auto solutions = solve_222(root);

//     solutions.sort_by_depth();
//     solutions.show();

//     return 0;
// }