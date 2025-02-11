#include <algorithm>    // std::reverse
#include <memory>       // std::shared_ptr
#include <type_traits>  // is_function_v
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
        assert(ret.size() == comments.size());
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

template <typename Initializer, typename Solver>
auto make_expander(const Initializer& initializer, const Solver& solver) {
    return [&initializer, &solver](const StepNode::sptr step_node,
                                   const unsigned max_depth) {
        auto step_cc = std::vector<StepNode::sptr>{};

        auto root = initializer(step_node->state);
        auto step_mbc = solver(root, max_depth);

        for (auto&& mbc_node : step_mbc) {
            Algorithm seq = mbc_node->get_path();
            CubieCube cc(step_node->state);
            cc.apply(seq);
            auto depth = step_node->depth + mbc_node->depth;
            step_cc.emplace_back(new StepNode{cc, seq, step_node, depth});
        }
        return step_cc;
    };
}

template <typename Expander, typename NextStepper>
auto make_stepper(const Expander& expander, const NextStepper& next_stepper) {
    return [&expander, &next_stepper](
               const std::vector<StepNode::sptr>& prev_step_cc,
               const unsigned move_budget,
               const unsigned breadth) -> std::vector<StepNode::sptr> {
        auto step_cc = std::vector<StepNode::sptr>{};
        step_cc.reserve(breadth);

        for (auto&& step_node : prev_step_cc) {
            unsigned depth = step_node->depth;
            auto children = expander(step_node, move_budget - step_node->depth);

            if (step_cc.size() + children.size() > breadth) {
                break;  // Do not expand more nodes if breadth is reached
            }
            step_cc.insert(step_cc.end(), children.begin(), children.end());
        }

        if constexpr (std::is_invocable_v<NextStepper,
                                          const std::vector<StepNode::sptr>&,
                                          unsigned, unsigned>) {
            return next_stepper(step_cc, move_budget, breadth);
        } else {
            return step_cc;
        }
    };
}

struct STEPFINAL {};

auto expand_step_three =
    make_expander(block_solver_F2Lm1::cc_initialize, block_solver_F2Lm1::solve);
auto expand_step_two =
    make_expander(block_solver_223::cc_initialize, block_solver_223::solve);
auto expand_step_one =
    make_expander(block_solver_222::cc_initialize, block_solver_222::solve);
auto make_step_three = make_stepper(expand_step_three, STEPFINAL{});
auto make_step_two = make_stepper(expand_step_two, make_step_three);
auto make_step_one = make_stepper(expand_step_one, make_step_two);

int main() {
    auto scramble = Algorithm(
        "R' U' F L2 D L' B R D' B' U' D2 L' U2 L B2 R2 B2 U2 F' L B2 R' U' F");
    scramble.show();

    CubieCube scramble_cc(scramble);
    auto root = std::make_shared<StepNode>(scramble_cc, Algorithm(), nullptr);
    auto solutions = make_step_one({root}, 14, 100);

    for (auto node : solutions) {
        std::cout << "------------------" << std::endl;
        node->get_skeleton({"2x2x2", "2x2x3", "F2L-1"}).show();
    }
    return 0;
}