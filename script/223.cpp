#include "move_table.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"
#include "symmetry.hpp"

// The optimal pruning table for the 2x2x3 is
// too big, so we use 1x2x3s instead. Solving the DB 2x2x3
// is just the same as solving the 2 1x2x3 that share the DB edge

constexpr unsigned NB = 24;
using Cube = MultiBlockCube<NB>;

auto block = Block<2, 3>("DB_123", {DLB, DRB}, {DB, RB, LB});

std::array<unsigned, NB> rotations = {
    symmetry_index(0, 0, 0, 0), symmetry_index(2, 3, 0, 0),  // DB
    symmetry_index(0, 1, 0, 0), symmetry_index(2, 0, 0, 0),  // DL
    symmetry_index(0, 2, 0, 0), symmetry_index(2, 1, 0, 0),  // DF
    symmetry_index(0, 3, 0, 0), symmetry_index(2, 2, 0, 0),  // DR
    symmetry_index(0, 0, 1, 0), symmetry_index(2, 3, 1, 0),  // UB
    symmetry_index(0, 1, 1, 0), symmetry_index(2, 0, 1, 0),  // UR
    symmetry_index(0, 3, 1, 0), symmetry_index(2, 2, 1, 0),  // UL
    symmetry_index(0, 2, 1, 0), symmetry_index(2, 1, 1, 0),  // UF
    symmetry_index(1, 0, 0, 0), symmetry_index(1, 3, 1, 0),  // LB
    symmetry_index(1, 1, 0, 0), symmetry_index(1, 2, 1, 0),  // LF
    symmetry_index(1, 3, 0, 0), symmetry_index(1, 0, 1, 0),  // RB
    symmetry_index(1, 2, 0, 0), symmetry_index(1, 1, 1, 0),  // RF
};

auto m_table = BlockMoveTable(block);
auto p_table = load_pruning_table(block);
auto apply = get_sym_apply<NB>(m_table, rotations);

unsigned max_estimate(const CoordinateBlockCube& cbc1,
                      const CoordinateBlockCube& cbc2) {
    return p_table.get_estimate(cbc1) > p_table.get_estimate(cbc2)
               ? p_table.get_estimate(cbc1)
               : p_table.get_estimate(cbc2);
}

auto estimate = [](const Cube& cube) {
    unsigned ret = max_estimate(cube[0], cube[1]);
    for (unsigned k = 0; 2 * k < NB; ++k) {
        unsigned this_223_estimate = max_estimate(cube[2 * k], cube[2 * k + 1]);
        ret = ret < this_223_estimate ? ret : this_223_estimate;
    }
    return ret;
};

auto is_solved = [](const Cube& cube) {
    for (unsigned k = 0; 2 * k < NB; ++k) {
        if (block.is_solved(cube[2 * k]) && block.is_solved(cube[2 * k + 1]))
            return true;
    }
    return false;
};

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto root = init_root(scramble, block, rotations);

    auto solutions = IDAstar(root, apply, estimate, is_solved);

    assert(is_solved(solutions[0]->state));
    solutions.sort_by_depth();
    solutions.show();
    return 0.;
}