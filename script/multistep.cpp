#include <algorithm>  // std::reverse
#include <memory>     // std::shared_ptr
#include <vector>

#include "222.hpp"
#include "223.hpp"
#include "F2L-1.hpp"
#include "algorithm.hpp"

struct StepNode {
    using sptr = std::shared_ptr<StepNode>;
    CubieCube state;
    Algorithm seq;
    sptr parent;
    unsigned depth;

    Skeleton get_skeleton(const std::vector<std::string>& comments) const {
        Skeleton ret;

        auto node = *this;
        while (node.parent) {
            ret.push_back(StepAlgorithm(node.seq));
            node = *node.parent;
        }
        assert(ret.size() ==
               comments.size());  // Ensure that the number of comments is the
        // same as the number of steps
        std::reverse(ret.begin(), ret.end());
        for (size_t i = 0; i < comments.size(); ++i) {
            ret[i].comment = comments[i];
        }
        return ret;
    }
    Skeleton get_skeleton(std::vector<std::string>&& comments) const {
        return get_skeleton(comments);
    }
};

std::vector<StepNode::sptr> expand_step_three(const StepNode::sptr step_node,
                                              const unsigned max_depth) {
    auto step_three_cc = std::vector<StepNode::sptr>{};

    auto root = block_solver_F2Lm1::cc_initialize(step_node->state);
    auto step_two_mbc = block_solver_F2Lm1::solve(root, max_depth);

    for (auto&& mbc_node : step_two_mbc) {
        Algorithm seq = mbc_node->get_path();
        CubieCube cc(step_node->state);
        cc.apply(seq);
        auto depth = step_node->depth + mbc_node->depth;
        step_three_cc.emplace_back(new StepNode{cc, seq, step_node, depth});
    }
    return step_three_cc;
}

std::vector<StepNode::sptr> make_step_three(
    const std::vector<StepNode::sptr>& step_two_cc, const unsigned move_budget,
    const unsigned breadth) {
    auto step_three_cc = std::vector<StepNode::sptr>{};
    step_three_cc.reserve(breadth);

    for (auto&& step_node : step_two_cc) {
        unsigned depth = step_node->depth;
        auto children =
            expand_step_three(step_node, move_budget - step_node->depth);
        if (step_three_cc.size() + children.size() > breadth) {
            break;  // Do not expand more nodes if breadth is reached
        }
        step_three_cc.insert(step_three_cc.end(), children.begin(),
                             children.end());
    }
    return step_three_cc;
}

std::vector<StepNode::sptr> expand_step_two(const StepNode::sptr step_node,
                                            const unsigned max_depth) {
    auto step_two_cc = std::vector<StepNode::sptr>{};

    auto root = block_solver_223::cc_initialize(step_node->state);
    auto step_one_mbc = block_solver_223::solve(root, max_depth);

    for (auto&& mbc_node : step_one_mbc) {
        Algorithm seq = mbc_node->get_path();
        CubieCube cc(step_node->state);
        cc.apply(seq);
        auto depth = step_node->depth + mbc_node->depth;
        step_two_cc.emplace_back(new StepNode{cc, seq, step_node, depth});
    }
    return step_two_cc;
}

std::vector<StepNode::sptr> make_step_two(
    const std::vector<StepNode::sptr>& step_one_cc, unsigned move_budget,
    const unsigned breadth) {
    auto step_two_cc = std::vector<StepNode::sptr>{};
    step_two_cc.reserve(breadth);

    for (auto&& step_node : step_one_cc) {
        unsigned depth = step_node->depth;
        auto children =
            expand_step_two(step_node, move_budget - step_node->depth);
        if (step_two_cc.size() + children.size() > breadth) {
            break;  // Do not expand more nodes if breadth is reached
        }
        step_two_cc.insert(step_two_cc.end(), children.begin(), children.end());
    }

    return make_step_three(step_two_cc, move_budget, breadth);
}

auto expand_step_one(const StepNode::sptr step_node, const unsigned max_depth) {
    auto step_one_cc = std::vector<StepNode::sptr>{};

    auto root = block_solver_222::cc_initialize(step_node->state);
    auto step_one_mbc = block_solver_222::solve(root, max_depth);

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
    return make_step_two(step_one_cc, move_budget, breadth);
}

int main() {
    auto scramble = Algorithm(
        "R' U' F U B2 L2 F2 R2 D2 B D2 F' R D2 U2 F2 D U' L U' R2 D' R' U' F");
    scramble.show();
    CubieCube scramble_cc(scramble);
    auto root = std::make_shared<StepNode>(scramble_cc, Algorithm(), nullptr);
    auto solutions = make_step_one(root, 14, 100);
    for (auto node : solutions) {
        std::cout << "------------------" << std::endl;
        node->get_skeleton({"2x2x2", "2x2x3", "F2L-1"}).show();
    }
    return 0;
}