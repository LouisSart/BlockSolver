#include "step.hpp"

#include "pruning_table.hpp"

Block<1, 0> block_1("ULF corner", {ULF}, {});
Block<0, 1> block_2("UF edge", {}, {UF});

void step_first_test() {
    auto mover =
        Mover{new BlockMoveTable(block_1), new BlockMoveTable(block_2)};
    auto apply = mover.get_apply();
    MultiBlockCube<2> cube;
    assert(cube.is_solved());
    apply(U, cube);
    assert(!cube.is_solved());
    Algorithm seq({R, U, D, B2});
    mover.apply(seq, cube);
    mover.apply(U, cube);

    auto pt1 = Strategy::Optimal(block_1).gen_table();
    auto pt2 = Strategy::Optimal(block_2).gen_table();
    auto pruner = Pruner(pt1.get_ptr(), pt2.get_ptr());
    auto estimator = pruner.get_estimator();
    assert(0 < pruner.get_estimate(cube));
    assert(0 < estimator(cube));
}

int main() {
    step_first_test();
    return 0;
}