#include <iostream>

#include "option.hpp"
#include "step.hpp"

constexpr unsigned NB = 2;   // the F2L-1 is splitted into 2 blocks
constexpr unsigned NS = 24;  // number of F2L-1 symmetries
using Cube = std::array<MultiBlockCube<NB>, NS>;
auto block1 = Block<3, 3>("222_w_extra_corners", {DLF, DLB, DRB}, {DL, LB, DB});
auto block2 = Block<2, 4>("2_squares", {DLF, DRB}, {LF, DF, RB, DR});

auto m_table1 = BlockMoveTable(block1);
auto m_table2 = BlockMoveTable(block2);
auto p_table1 = load_pruning_table(block1);
auto p_table2 = load_pruning_table(block2);

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

void local_apply(const Move& move, const unsigned& sym,
                 MultiBlockCube<NB>& subcube) {
    m_table1.sym_apply(move, sym, subcube[0]);
    m_table2.sym_apply(move, sym, subcube[1]);
};

void apply(const Move& move, Cube& cube) {
    for (unsigned k = 0; k < NS; ++k) {
        local_apply(move, rotations[k], cube[k]);
    }
};

unsigned max_estimate(const MultiBlockCube<NB>& cube) {
    return p_table1.get_estimate(cube[0]) > p_table2.get_estimate(cube[1])
               ? p_table1.get_estimate(cube[0])
               : p_table2.get_estimate(cube[1]);
}

auto estimate = [](const Cube& cube) {
    unsigned ret = max_estimate(cube[0]);
    for (unsigned k = 0; k < NS; ++k) {
        unsigned e = max_estimate(cube[k]);
        ret = ret < e ? ret : e;
    }
    return ret;
};

bool local_is_solved(const MultiBlockCube<NB>& subcube) {
    return (block1.is_solved(subcube[0]) && block2.is_solved(subcube[1]));
}

auto is_solved = [](const Cube& cube) {
    for (unsigned k = 0; k < NS; ++k) {
        if (local_is_solved(cube[k])) return true;
    }
    return false;
};

auto my_init_root(const Algorithm& scramble) {
    Cube ret;
    CubieCube scramble_cc(scramble);

    for (unsigned k = 0; k < NS; ++k) {
        ret[k][0] = block1.to_coordinate_block_cube(
            scramble_cc.get_conjugate(rotations[k]));
        ret[k][1] = block2.to_coordinate_block_cube(
            scramble_cc.get_conjugate(rotations[k]));
    }

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