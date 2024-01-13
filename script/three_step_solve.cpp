#include <tuple>

#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"

Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                    U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
Block<1, 3> block_1("DLB_222", {DLB}, {DL, LB, DB});
Block<1, 2> block_2("DL_F_sq", {DLF}, {DF, LF});
Block<1, 2> block_3("DB_R_sq", {DRB}, {RB, DR});

auto mover = Mover(new BlockMoveTable(block_1), new BlockMoveTable(block_2),
                   new BlockMoveTable(block_3));

auto pruner = Pruner(load_table_ptr(Strategy::Optimal(block_1)),
                     load_table_ptr(Strategy::Optimal(block_2)),
                     load_table_ptr(Strategy::Optimal(block_3)));

template <unsigned step>
auto make_step(const Node<MultiBlockCube<3>>::sptr root, unsigned step_depth) {
    return IDAstar<false>(root, mover.get_apply(), pruner.get_estimator<step>(),
                          get_is_solved(root->state, step));
}

int main() {
    scramble.show();
    MultiBlockCube<3> cube;
    mover.apply(scramble, cube);

    // Step 1
    std::cout << "\nStep 1: " << block_1.name << std::endl;
    auto root = Node<MultiBlockCube<3>>::make_root(cube);
    auto solutions =
        IDAstar<true>(root, mover.get_apply(), pruner.get_estimator<0>(),
                      get_is_solved(cube, 0));
    show(solutions);

    // Step 2
    std::cout << "\nStep 2: " << block_2.name << std::endl;
    auto step2_solutions = Solutions<typename Node<MultiBlockCube<3>>::sptr>();
    for (auto node : solutions) {
        auto tmp = make_step<1>(node, 7);
        step2_solutions.insert(step2_solutions.begin(), tmp.begin(), tmp.end());
    }
    show(step2_solutions);

    // Step 3
    std::cout << "\nStep 3: " << block_3.name << std::endl;
    MultiBlockCube<3> cube3;
    mover.apply(step2_solutions[0]->get_path(), cube3);
    auto root3 = Node<MultiBlockCube<3>>::make_root(cube3);
    auto step3_solutions = make_step<2>(root3, 10);
    // IDAstar(root3, mover.get_apply(), pruner.get_estimator<2>(),
    //         get_is_solved(root3->state, 2));
    return 0;
}