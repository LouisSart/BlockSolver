#include <tuple>

#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"

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
    return depth_first_search<false>(
        root, mover.get_apply(), pruner.get_estimator<step>(),
        get_is_solved(root->state, step), step_depth);
}

using MultiNode = Node<MultiBlockCube<3>>;
using NodePtr = MultiNode::sptr;

int main() {
    scramble.show();
    MultiBlockCube<3> cube;
    mover.apply(scramble, cube);

    // Step 1 : 2x2x2
    auto root = MultiNode::make_root(cube);
    auto solutions = make_step<0>(root, 6);

    // Step 2 : 2x2x3
    Solutions<NodePtr> step2_solutions;
    for (auto node : solutions) {
        auto tmp = make_step<1>(node, 11);
        step2_solutions.insert(step2_solutions.end(), tmp.begin(), tmp.end());
    }

    // Step 3 : F2L-1
    Solutions<NodePtr> step3_solutions;
    for (auto node : step2_solutions) {
        auto tmp = make_step<2>(node, 16);
        step3_solutions.insert(step3_solutions.end(), tmp.begin(), tmp.end());
    }
    std::cout << "Three step F2L-1 solutions" << std::endl;
    show(step3_solutions);
    return 0;
}