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

std::array<unsigned, 3> splits_move_counts{8, 12, 15};

template <unsigned step>
auto make_step(const Node<MultiBlockCube<3>>::sptr root, unsigned step_depth) {
    return depth_first_search<false>(
        root, mover.get_apply(), pruner.get_estimator<step>(),
        get_is_solved(root->state, step), step_depth);
}

using MultiNode = Node<MultiBlockCube<3>>;
using NodePtr = MultiNode::sptr;
using StepMultiNode = StepNode<MultiBlockCube<3>>;
using StepNodePtr = StepMultiNode::sptr;

auto expand(const StepNodePtr step_node_ptr) {
    NodePtr node =
        MultiNode::make_node(step_node_ptr->state, step_node_ptr->depth);
    Solutions<NodePtr> solutions;
    switch (step_node_ptr->depth) {
        case 0:
            solutions = make_step<0>(node, splits_move_counts[0]);
        case 1:
            solutions = make_step<1>(node, splits_move_counts[1]);
        case 2:
            solutions = make_step<2>(node, splits_move_counts[2]);
        default:
            std::cout << "Error in StepNode expansion" << std::endl;
            abort();
    }
    Solutions<StepNodePtr> ret;
    for (auto&& node_ptr : solutions) {
        auto child = StepMultiNode::make_node(node_ptr->state, node_ptr->depth);
        child->parent = step_node_ptr;
        child->path = node_ptr->get_path();
        child->number = step_node_ptr->number + 1;
        ret.push_back(child);
    }
    return ret;
}

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm::make_from_str(argv[argc - 1]);
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