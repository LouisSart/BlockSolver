#include <tuple>

#include "move_table.hpp"
#include "pruning_table.hpp"
#include "search.hpp"

Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B, U3,
                    D3, F2, B2, U,  L2, D,  F2, U2, D,  R3, U3, F});
Block<1, 3> block_1("DLB_222", {DLB}, {DL, LB, DB});
Block<1, 2> block_2("DL_F_sq", {DLF}, {DF, LF});
Block<1, 2> block_3("DB_R_sq", {DRB}, {RB, DR});
auto m_tables = std::make_tuple(
    BlockMoveTable(block_1), BlockMoveTable(block_2), BlockMoveTable(block_3));

namespace Mover {
template <unsigned step, unsigned n_steps>
auto apply = [](const Move& move, MultiBlockCube<n_steps>& cube) {
    std::get<step>(m_tables).apply(move, cube[step]);
    if constexpr (step > 0) {
        apply<step - 1, n_steps>(move, cube);
    }
};

template <unsigned step, unsigned n_steps>
auto apply_seq = [](const Algorithm& seq, MultiBlockCube<n_steps>& cube) {
    std::get<step>(m_tables).apply(seq, cube[step]);
    if constexpr (step > 0) {
        apply_seq<step - 1, n_steps>(seq, cube);
    }
};
}  // namespace Mover

auto p_tables = std::tuple(Strategy::Optimal(block_1).load_table(),
                           Strategy::Optimal(block_2).load_table(),
                           Strategy::Optimal(block_3).load_table());

template <unsigned step, unsigned nsteps>
auto h = [](const MultiBlockCube<nsteps>& state) {
    static_assert(nsteps > step);
    if constexpr (step == 0) {
        return std::get<0>(p_tables).get_estimate(state[0]);
    } else {
        auto this_step_estimate =
            std::get<step>(p_tables).get_estimate(state[step]);
        return (this_step_estimate > h<step - 1, nsteps>(state))
                   ? this_step_estimate
                   : h<step - 1, nsteps>(state);
    }
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
    Mover::apply_seq<2, 3>(scramble, cube);

    // Step 1
    std::cout << "\nStep 1: " << block_1.name << std::endl;
    auto root = Node(cube, 0);
    auto solutions =
        IDAstar<false>(root, Mover::apply<2, 3>, h<0, 3>, is_solved<0, 3>);
    show(solutions);

    // Step 2
    std::cout << "\nStep 2: " << block_2.name << std::endl;
    Mover::apply_seq<2, 3>(solutions[0], cube);  // take first solution found
    root = Node(cube, 0);
    solutions =
        IDAstar<false>(root, Mover::apply<2, 3>, h<1, 3>, is_solved<1, 3>, 6);
    show(solutions);

    // Step 3
    std::cout << "\nStep 3: " << block_3.name << std::endl;
    Mover::apply_seq<2, 3>(solutions[0], cube);  // take first solution found
    root = Node(cube, 0);
    solutions =
        IDAstar<false>(root, Mover::apply<2, 3>, h<2, 3>, is_solved<2, 3>, 7);
    show(solutions);
    return 0;
}