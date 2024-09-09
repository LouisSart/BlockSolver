#include "move_table.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"
#include "symmetry.hpp"

constexpr unsigned NB = 8;  // There are 8 different 2x2x2 to solve
using Cube = MultiBlockCube<NB>;

auto block = Block<1, 3>("DLB_222", {DLB}, {LB, DB, DL});

std::array<unsigned, NB> rotations = {
    symmetry_index(0, 0, 0, 0), symmetry_index(0, 1, 0, 0),
    symmetry_index(0, 2, 0, 0), symmetry_index(0, 3, 0, 0),
    symmetry_index(0, 0, 1, 0), symmetry_index(0, 1, 1, 0),
    symmetry_index(0, 2, 1, 0), symmetry_index(0, 3, 1, 0)};

auto m_table = BlockMoveTable(block);
auto p_table = load_pruning_table(block);
auto apply = get_sym_apply<NB>(m_table, rotations);
auto estimate = get_estimator<NB>(p_table);
auto is_solved = get_is_solved<NB>(block);

int main(int argc, const char *argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto root = init_root(scramble, block, rotations);

    auto solutions = IDAstar(root, apply, estimate, is_solved);

    solutions.sort_by_depth();
    solutions.show();
    return 0;
}