#include "move_table.hpp"
#include "pruning_table.hpp"
#include "search.hpp"

template <unsigned nb>
struct MultiBlockCube : std::array<CoordinateBlockCube, nb> {
    void show() const {
        std::cout << "MultiBlockCube<" << nb << ">" << std::endl;
        for (unsigned k = 0; k < nb; ++k) {
            std::cout << "   Block " << k << ": ";
            if ((*this)[k].is_solved()) {
                std ::cout << "solved" << std::endl;
            } else {
                std ::cout << "not solved" << std::endl;
            }
        }
    }
};

Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                    U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
Block<1, 3> block_1("DLB_222", {DLB}, {DL, LB, DB});
Block<1, 2> block_2("DL_F_sq", {DLF}, {DF, LF});
BlockMoveTable mt1(block_1);
BlockMoveTable mt2(block_2);

namespace Mover {
auto apply = [](const Move& move, MultiBlockCube<2>& cube) {
    mt1.apply(move, cube[0]);
    mt2.apply(move, cube[1]);
};

auto apply_seq = [](const Algorithm& seq, MultiBlockCube<2>& cube) {
    mt1.apply(seq, cube[0]);
    mt2.apply(seq, cube[1]);
};
}  // namespace Mover

auto pt1 = Strategy::Optimal(block_1).load_table();
auto pt2 = Strategy::Optimal(block_2).load_table();

auto h1 = [](const MultiBlockCube<2>& state) {
    return pt1.get_estimate(state[0]);
};

auto h2 = [](const MultiBlockCube<2>& state) {
    return (pt1.get_estimate(state[0]) < pt2.get_estimate(state[1]))
               ? pt2.get_estimate(state[1])
               : pt1.get_estimate(state[0]);
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
    MultiBlockCube<2> cube;
    cube.show();
    Mover::apply_seq(scramble, cube);
    cube.show();

    // Step 1
    auto root = Node(cube, 0);
    auto solutions =
        depth_first_search(root, Mover::apply, h1, is_solved<0, 2>);
    show(solutions);

    // Step 2
    Mover::apply_seq(solutions[0], cube);  // take first solution found
    root = Node(cube, 0);
    solutions = depth_first_search(root, Mover::apply, h2, is_solved<1, 2>, 7);
    show(solutions);
    return 0;
}