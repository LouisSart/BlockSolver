#include <tuple>

#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"

Algorithm scramble({R3, U3, F,  R2, B,  D, B,  L2, U3, R3, D2, L,  F,
                    U2, F3, L2, F3, D2, F, D2, R2, B3, D2, R3, U3, F});

Block<1, 3> block_1("DLB_222", {DLB}, {DL, LB, DB});
Block<1, 2> block_2("DL_F_sq", {DLF}, {DF, LF});
Block<1, 2> block_3("DB_R_sq", {DRB}, {RB, DR});

auto mover = Mover(new BlockMoveTable(block_1), new BlockMoveTable(block_2),
                   new BlockMoveTable(block_3));

auto pruner = Pruner(load_table_ptr(Strategy::Optimal(block_1)),
                     load_table_ptr(Strategy::Optimal(block_2)),
                     load_table_ptr(Strategy::Optimal(block_3)));

int main() {
    scramble.show();
    MultiBlockCube<3> cube;
    mover.apply(scramble, cube);

    // Step 1
    std::cout << "\nStep 1: " << block_1.name << std::endl;
    auto root = Node(cube, 0);
    auto solutions =
        IDAstar<true>(root, mover.get_apply(), pruner.get_estimator<0>(),
                      get_is_solved(cube, 0));
    show(solutions);

    // Step 2
    std::cout << "\nStep 2: " << block_2.name << std::endl;
    mover.apply(solutions[0], cube);  // take first solution found
    root = Node(cube, 0);
    solutions =
        IDAstar<false>(root, mover.get_apply(), pruner.get_estimator<1>(),
                       get_is_solved(cube, 1), 6);
    show(solutions);

    // Step 3
    std::cout << "\nStep 3: " << block_3.name << std::endl;
    mover.apply(solutions[0], cube);  // take first solution found
    root = Node(cube, 0);
    solutions =
        IDAstar<false>(root, mover.get_apply(), pruner.get_estimator<2>(),
                       get_is_solved(cube, 2));
    show(solutions);
    return 0;
}