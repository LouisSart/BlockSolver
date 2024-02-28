#include "step.hpp"

#include "pruning_table.hpp"

Block<1, 0> block_1("ULF corner", {ULF}, {});
Block<0, 1> block_2("UF edge", {}, {UF});

auto mover = Mover{new BlockMoveTable(block_1), new BlockMoveTable(block_2)};
auto pruner = Pruner(gen_table_ptr(Strategy::Optimal(block_1)),
                     gen_table_ptr(Strategy::Optimal(block_2)));
auto apply = mover.get_apply();

void step_first_test() {
    MultiBlockCube<2> cube;
    assert(cube.is_solved());

    apply(L, cube);  // Only breaks block1
    assert(!is_solved<0>(cube));
    assert(is_solved<1>(cube));
    assert((!is_solved<0, 1>(cube)));
    auto one_step_estimator = pruner.get_estimator<0>();
    assert(0 < pruner.get_estimate<0>(cube));
    assert(0 < one_step_estimator(cube));
    auto second_step_estimator = pruner.get_estimator<1>();
    assert(0 == pruner.get_estimate<1>(cube));
    assert(0 == second_step_estimator(cube));
    auto two_step_estimator = pruner.get_estimator<0, 1>();
    assert((0 < pruner.get_estimate<0, 1>(cube)));
    assert(0 < two_step_estimator(cube));

    Algorithm seq({R, U, D, B2});
    mover.apply(seq, cube);
    mover.apply(U, cube);
    assert(2 == one_step_estimator(cube));
    assert(1 == second_step_estimator(cube));
    assert(2 == two_step_estimator(cube));
}

void second_step_test() {
    MultiBlockCube<2> cube;
    auto root = Node<MultiBlockCube<2>>::make_root(cube);
    auto children = root->expand(mover.get_apply(),
                                 pruner.get_estimator<0, 1>(), HTM_Moves);
    assert(children[R]->estimate == 0);
    assert(children[U]->estimate == 1);
    assert(children[F]->estimate == 1);
    assert(children[L]->estimate == 1);
}

void step_object_test() {
    MultiBlockCube<2> cube;
    auto step_root = StepNode<MultiBlockCube<2>>::make_root(cube);
    step_root->show();
}

int main() {
    step_first_test();
    second_step_test();
    step_object_test();
    return 0;
}