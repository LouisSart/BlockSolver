#include "move_table.hpp"
#include "pruning_table.hpp"
#include "search.hpp"

Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B, U3,
                    D3, F2, B2, U,  L2, D,  F2, U2, D,  R3, U3, F});
Block<1, 3> block_1("DLB_222", {DLB}, {DL, LB, DB});
Block<1, 2> block_2("DL_F_sq", {DLF}, {DF, LF});
Block<1, 2> block_3("DB_R_sq", {DRB}, {RB, DR});
BlockMoveTable mt1(block_1);
BlockMoveTable mt2(block_2);
BlockMoveTable mt3(block_3);

namespace Mover {
auto apply = [](const Move& move, MultiBlockCube<3>& cube) {
    mt1.apply(move, cube[0]);
    mt2.apply(move, cube[1]);
    mt3.apply(move, cube[2]);
};

auto apply_seq = [](const Algorithm& seq, MultiBlockCube<3>& cube) {
    mt1.apply(seq, cube[0]);
    mt2.apply(seq, cube[1]);
    mt3.apply(seq, cube[2]);
};
}  // namespace Mover

auto pt1 = Strategy::Optimal(block_1).load_table();
auto pt2 = Strategy::Optimal(block_2).load_table();
auto pt3 = Strategy::Optimal(block_3).load_table();

auto h1 = [](const MultiBlockCube<3>& state) {
    return pt1.get_estimate(state[0]);
};

auto h2 = [](const MultiBlockCube<3>& state) {
    return (pt2.get_estimate(state[1]) > h1(state)) ? pt2.get_estimate(state[1])
                                                    : h1(state);
};

auto h3 = [](const MultiBlockCube<3>& state) {
    return (pt3.get_estimate(state[2]) > h2(state)) ? pt3.get_estimate(state[2])
                                                    : h2(state);
};

template <unsigned step, unsigned nsteps>
auto is_solved = [](const MultiBlockCube<nsteps>& cube) {
    static_assert(step < nsteps);
    if constexpr (step == 0) {
        return cube[0].is_solved();
    } else {
        return (cube[step].is_solved() && is_solved<step - 1, nsteps>(cube));
    }
};

int main() {
    scramble.show();
    MultiBlockCube<3> cube;
    Mover::apply_seq(scramble, cube);

    // Step 1
    std::cout << "\nStep 1: " << block_1.name << std::endl;
    auto root = Node(cube, 0);
    auto solutions = IDAstar<false>(root, Mover::apply, h1, is_solved<0, 3>);
    show(solutions);

    // Step 2
    std::cout << "\nStep 2: " << block_2.name << std::endl;
    Mover::apply_seq(solutions[0], cube);  // take first solution found
    root = Node(cube, 0);
    solutions = IDAstar<false>(root, Mover::apply, h2, is_solved<1, 3>, 6);
    show(solutions);

    // Step 3
    std::cout << "\nStep 3: " << block_3.name << std::endl;
    Mover::apply_seq(solutions[0], cube);  // take first solution found
    root = Node(cube, 0);
    solutions = IDAstar<false>(root, Mover::apply, h3, is_solved<2, 3>, 7);
    show(solutions);
    return 0;
}