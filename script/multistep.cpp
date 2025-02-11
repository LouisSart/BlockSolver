#include <memory>
#include <vector>

#include "222.hpp"

struct StepNode {  // : std::enable_shared_from_this<StepNode> {
    using sptr = std::shared_ptr<StepNode>;
    CubieCube state;
    Algorithm seq;
    sptr parent;
};

std::vector<StepNode::sptr> make_step_two(
    const std::vector<StepNode::sptr>& step_one_cc, const unsigned move_budget,
    const unsigned breadth) {
    auto step_two_cc = std::vector<StepNode::sptr>{};

    // for (auto&& node : step_one_cc) {
    //     auto root = block_solver_222::cc_initialize(node->state);
    //     auto step_two_mbc = block_solver_222::solve(root, move_budget);

    //     for (auto&& mbc_node : step_two_mbc) {
    //         Algorithm seq = mbc_node->get_path();
    //         CubieCube cc(seq);
    //         step_two_cc.emplace_back(new StepNode{cc, seq, node});
    //     }
    // }
    return step_two_cc;
}

auto expand_step_one(const StepNode::sptr node, const unsigned move_budget) {
    auto step_one_cc = std::vector<StepNode::sptr>{};

    auto root = block_solver_222::cc_initialize(node->state);
    auto step_one_mbc = block_solver_222::solve(root, move_budget);

    for (auto&& mbc_node : step_one_mbc) {
        Algorithm seq = mbc_node->get_path();
        CubieCube cc(seq);
        step_one_cc.emplace_back(new StepNode{cc, seq, node});
    }
    return step_one_cc;
}

auto make_step_one(const Algorithm& scramble, unsigned move_budget,
                   const unsigned breadth) {
    CubieCube scramble_cc(scramble);
    auto root = std::make_shared<StepNode>(scramble_cc, Algorithm(), nullptr);

    auto step_one_cc = expand_step_one(root, move_budget);
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
    auto scramble = Algorithm("U2 R2 B F2 U D2 R L2 D' B2");
    CubieCube scramble_cc(scramble);
    auto root = std::make_shared<StepNode>(scramble_cc, Algorithm(), nullptr);
    auto solutions = expand_step_one(root, 5);
    for (auto node : solutions) {
        node->seq.show();
    }
    return 0;
}