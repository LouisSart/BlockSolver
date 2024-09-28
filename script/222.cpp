#include "move_table.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"
#include "symmetry.hpp"
#include "utils.hpp"

template <unsigned nc, unsigned ne, long unsigned NS>
auto make_optimal_block_solver(Block<nc, ne> &block,
                               const std::array<unsigned, NS> &rotations) {
    static auto m_table = BlockMoveTable(block);
    static auto p_table = load_pruning_table(block);
    static auto apply = get_sym_apply<NS>(m_table, rotations);
    static auto estimate = get_estimator<NS>(p_table);
    static auto is_solved = get_is_solved<NS>(block);

    return [](const auto node) {
        print(estimate(node->state), is_solved(node->state));
        // return 1;
        return IDAstar(node, apply, estimate, is_solved);
    };
}

constexpr unsigned NS = 8;  // There are 8 different 2x2x2 symmetries
// using Cube = MultiBlockCube<NS>;

auto block = Block<1, 3>("DLB_222", {DLB}, {LB, DB, DL});

std::array<unsigned, NS> rotations = {
    symmetry_index(0, 0, 0, 0), symmetry_index(0, 1, 0, 0),
    symmetry_index(0, 2, 0, 0), symmetry_index(0, 3, 0, 0),
    symmetry_index(0, 0, 1, 0), symmetry_index(0, 1, 1, 0),
    symmetry_index(0, 2, 1, 0), symmetry_index(0, 3, 1, 0)};

auto solve_222 = make_optimal_block_solver(block, rotations);

int main(int argc, const char *argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto root = init_root(scramble, block, rotations);

    auto solutions = solve_222(root);

    solutions.sort_by_depth();
    solutions.show();
    return 0;
}