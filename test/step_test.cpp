#include "move_table.hpp"
#include "pruning_table.hpp"

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
void apply(const Move& move, MultiBlockCube<2>& cube) {
    mt1.apply(move, cube[0]);
    mt2.apply(move, cube[1]);
};

void apply(const Algorithm& seq, MultiBlockCube<2>& cube) {
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
bool is_solved(const MultiBlockCube<nsteps>& cube) {
    static_assert(step < nsteps);
    if constexpr (step == 0) {
        return cube[0].is_solved();
    } else {
        return (cube[step].is_solved() && is_solved<step - 1>(cube));
    }
}

int main() {
    MultiBlockCube<2> cube;
    cube.show();
    Mover::apply(scramble, cube);
    cube.show();
    Algorithm step1_sol({D3, L2, B, L3});
    Mover::apply(step1_sol, cube);
    std::cout << is_solved<0>(cube) << " " << is_solved<1>(cube) << std::endl;
    return 0;
}