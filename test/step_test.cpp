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
    apply(U, cube);
    assert(!cube.is_solved());
    Algorithm seq({R, U, D, B2});
    mover.apply(seq, cube);
    mover.apply(U, cube);

    auto estimator = pruner.get_estimator<0>();
    assert(0 < pruner.get_estimate<0>(cube));
    assert(0 < estimator(cube));

    auto is_solved = get_is_solved(cube, 0);
    assert(!is_solved(cube));
}

void second_step_test() {
    MultiBlockCube<2> cube;
    auto root = Node<MultiBlockCube<2>>::make_root(cube);
    auto children =
        root->expand(mover.get_apply(), pruner.get_estimator<1>(), HTM_Moves);
    assert(children[R]->estimate == 0);
    assert(children[U]->estimate == 1);
    assert(children[F]->estimate == 1);
    assert(children[L]->estimate == 1);
}

int main() {
    step_first_test();
    second_step_test();
    return 0;
}