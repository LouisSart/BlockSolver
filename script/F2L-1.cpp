#include <iostream>

#include "option.hpp"
#include "step.hpp"

constexpr unsigned NB = 2;   // the F2L-1 is splitted into 2 blocks
constexpr unsigned NS = 24;  // number of F2L-1 symmetries
using Cube = MultiBlockCube<NB>;
auto block1 = Block<3, 3>("222_w_extra_corners", {DLF, DLB, DRB}, {DL, LB, DB});
auto block2 = Block<2, 4>("2_squares", {DLF, DRB}, {LF, DF, RB, DR});

auto m_table1 = BlockMoveTable(block1);
auto m_table2 = BlockMoveTable(block2);
auto p_table1 = load_pruning_table(block1);
auto p_table2 = load_pruning_table(block2);

std::array<unsigned, 1> rotations{symmetry_index(0, 0, 0, 0)};

auto apply = [](const Move& move, Cube& cube) {
    m_table1.apply(move, cube[0]);
    m_table2.apply(move, cube[1]);
};

unsigned max_estimate(const CoordinateBlockCube& cbc1,
                      const CoordinateBlockCube& cbc2) {
    return p_table1.get_estimate(cbc1) > p_table2.get_estimate(cbc2)
               ? p_table1.get_estimate(cbc1)
               : p_table2.get_estimate(cbc2);
}

auto estimate = [](const Cube& cube) {
    unsigned ret = max_estimate(cube[0], cube[1]);
    // for (unsigned k = 0; 2 * k < NB; ++k) {
    //     unsigned e = max_estimate(cube[2 * k], cube[2 * k + 1]);
    //     ret = ret < e ? ret : e;
    // }
    return ret;
};

auto is_solved = [](const Cube& cube) {
    // for (unsigned k = 0; 2 * k < NB; ++k) {
    // if (block.is_solved(cube[2 * k]) && block.is_solved(cube[2 * k + 1]))
    //     return true;
    return (block1.is_solved(cube[0]) && block2.is_solved(cube[1]));
    // return false;
};

auto my_init_root(const Algorithm& scramble) {
    Cube ret;
    CubieCube scramble_cc(scramble);

    ret[0] = block1.to_coordinate_block_cube(scramble_cc);
    ret[1] = block2.to_coordinate_block_cube(scramble_cc);

    return make_root(ret);
}

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto root = my_init_root(scramble);

    auto solutions = IDAstar(root, apply, estimate, is_solved);

    solutions.sort_by_depth();
    solutions.show();
    return 0;
}