#include <memory>
#include <vector>

#include "222.hpp"
#include "223.hpp"

struct StepNode {  // : std::enable_shared_from_this<StepNode> {
    using sptr = std::shared_ptr<StepNode>;
    CubieCube state;
    Algorithm seq;
    sptr parent;
    unsigned depth;
};

std::vector<StepNode::sptr> expand_step_two(const StepNode::sptr step_node,
                                            const unsigned move_budget) {
    auto step_two_cc = std::vector<StepNode::sptr>{};

    auto root = block_solver_223::cc_initialize(step_node->state);
    auto step_one_mbc =
        block_solver_223::solve(root, move_budget - step_node->depth);

    for (auto&& mbc_node : step_one_mbc) {
        Algorithm seq = mbc_node->get_path();
        CubieCube cc(seq);
        auto depth = root->depth + mbc_node->depth;
        step_two_cc.emplace_back(new StepNode{cc, seq, step_node, depth});
    }
    return step_two_cc;
}

std::vector<StepNode::sptr> make_step_two(
    const std::vector<StepNode::sptr>& step_one_cc, const unsigned move_budget,
    const unsigned breadth) {
    auto step_two_cc = std::vector<StepNode::sptr>{};
    step_two_cc.reserve(breadth);

    for (auto&& node_step_one : step_one_cc) {
        unsigned depth = node_step_one->depth;
        auto children_step_two = expand_step_two(node_step_one, move_budget);
        step_two_cc.insert(step_two_cc.end(), children_step_two.begin(),
                           children_step_two.end());
    }
    return step_two_cc;
}

auto expand_step_one(const StepNode::sptr step_node,
                     const unsigned move_budget) {
    auto step_one_cc = std::vector<StepNode::sptr>{};

    auto root = block_solver_222::cc_initialize(step_node->state);
    auto step_one_mbc = block_solver_222::solve(root, move_budget);

    for (auto&& mbc_node : step_one_mbc) {
        Algorithm seq = mbc_node->get_path();
        CubieCube cc = step_node->state;
        cc.apply(seq);
        unsigned depth = mbc_node->depth;
        step_one_cc.emplace_back(new StepNode{cc, seq, step_node, depth});
    }
    return step_one_cc;
}

auto make_step_one(const StepNode::sptr step_root, unsigned move_budget,
                   const unsigned breadth) {
    auto step_one_cc = expand_step_one(step_root, move_budget);
    auto solutions = make_step_two(step_one_cc, move_budget, breadth);

    if (step_one_cc.size() > breadth) {
        while (solutions.size() == 0) {
            move_budget += 1;
            solutions = make_step_two(step_one_cc, move_budget, breadth);
        }
    }
    return solutions;
}

int main() {
    auto scramble = Algorithm(
        "R' U' F R' U' R F2 L U2 B R' B D' U2 B2 R2 B' U2 R2 F U2 B R' U' F");
    CubieCube scramble_cc(scramble);
    auto root = std::make_shared<StepNode>(scramble_cc, Algorithm(), nullptr);
    auto solutions = make_step_one(root, 10, 1);
    for (auto node : solutions) {
        std::cout << node->parent->seq << node->seq << std::endl;
    }
    return 0;
}