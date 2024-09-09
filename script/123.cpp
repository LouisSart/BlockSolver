#include "move_table.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "symmetry.hpp"

constexpr unsigned NB = 24;  // There are 24 different 1x2x3 to solve
using Cube = MultiBlockCube<NB>;

auto block = Block<2, 3>("DB_123", {DLB, DRB}, {DB, RB, LB});
auto m_table = BlockMoveTable(block);
auto p_table = load_pruning_table(block);

std::vector<unsigned> rotations = {
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

auto init_root(const Algorithm &scramble) {
    CubieCube scramble_cc(scramble);
    Cube ret;

    for (unsigned k = 0; k < NB; ++k) {
        ret[k] = block.to_coordinate_block_cube(
            scramble_cc.get_conjugate(rotations[k]));
    }
    return make_root(ret);
}

auto apply = [](const Move &move, Cube &cube) {
    for (unsigned k = 0; k < NB; ++k) {
        m_table.sym_apply(move, rotations[k], cube[k]);
    }
};

auto estimate = [](const Cube &cube) {
    // Return the minimum estimate over all the possible 2x2x2s
    unsigned ret = p_table.get_estimate(cube[0]);
    for (unsigned k = 0; k < NB; ++k) {
        auto e = p_table.get_estimate(cube[k]);
        ret = (e < ret) ? e : ret;
    }
    return ret;
};

auto is_solved = [](const Cube &cube) {
    // Returns true if at least one of the cbc is solved
    static auto solved = block.to_coordinate_block_cube(CubieCube());

    for (auto cbc : cube) {
        if (block.is_solved(cbc)) return true;
    }
    return false;
};

int main(int argc, const char *argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto root = init_root(scramble);

    auto solutions = IDAstar(root, apply, estimate, is_solved);

    solutions.sort_by_depth();
    solutions.show();
    return 0.;
}