#include "step.hpp"

#include "pruning_table.hpp"

using Cube = MultiBlockCube<2>;
using Sol = Solutions<Node<Cube>::sptr>;
Block<1, 0> block_1("ULF corner", {ULF}, {});
Block<0, 1> block_2("UF edge", {}, {UF});

auto mover = Mover{new BlockMoveTable(block_1), new BlockMoveTable(block_2)};
auto pruner = Pruner(load_table_ptr(Strategy::Optimal(block_1)),
                     load_table_ptr(Strategy::Optimal(block_2)));
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
    auto root = make_root(cube);
    auto estimate = pruner.get_estimator<0, 1>();

    auto children = root->expand(mover.get_apply(), HTM_Moves);
    assert(estimate(children[R]->state) == 0);
    assert(estimate(children[U]->state) == 1);
    assert(estimate(children[F]->state) == 1);
    assert(estimate(children[L]->state) == 1);
}

void step_object_test() {
    MultiBlockCube<2> cube;
    mover.apply({L, U3}, cube);
    auto root = make_root(cube);
    root->show();

    auto step = make_step<1>(mover, pruner);
    auto solutions = step->solve_optimal(root);
    std::cout << "Step 1:" << std::endl;
    solutions.show();

    auto step2 = make_step<0, 1>(mover, pruner);
    solutions = step2->solve_optimal({solutions[0]});
    std::cout << "Step 2:" << std::endl;
    solutions.show();
}

void multi_phase_test() {
    std::cout << "Multiphase:" << std::endl;
    MultiBlockCube<2> cube;
    mover.apply({L, F, L, R, D}, cube);
    auto root = make_root(cube);

    auto step = make_step<1>(mover, pruner);
    auto step2 = make_step<0, 1>(mover, pruner);
    std::vector<StepBase<typename Node<Cube>::sptr, decltype(mover)>*> steps = {
        step, step2};

    auto solutions = multi_phase_search(Sol({root}), steps, {1, 1}, 0);
    solutions.show();
}

int main() {
    step_first_test();
    second_step_test();
    step_object_test();
    multi_phase_test();
    return 0;
}