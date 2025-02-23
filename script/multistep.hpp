#include <algorithm>    // std::reverse
#include <memory>       // std::shared_ptr
#include <type_traits>  // is_function_v
#include <vector>

#include "222.hpp"
#include "223.hpp"
#include "F2L-1.hpp"
#include "algorithm.hpp"

struct StepNode : std::enable_shared_from_this<StepNode> {
    using sptr = std::shared_ptr<StepNode>;
    CubieCube state;
    Algorithm seq;
    sptr parent;
    unsigned depth;

    StepNode(const CubieCube& state, const Algorithm& seq = "",
             sptr parent = nullptr, unsigned depth = 0)
        : state(state), seq(seq), parent(parent), depth(depth) {}

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

    template <typename Initializer, typename Solver>
    auto expand(const Initializer& initialize, const Solver& solve,
                const unsigned max_depth) {
        auto children = std::vector<StepNode::sptr>{};

        auto root = initialize(state);
        auto step_mbc = solve(root, max_depth);

        for (auto&& mbc_node : step_mbc) {
            Algorithm seq = mbc_node->get_path();
            seq.inv_flag = this->seq.inv_flag;

            CubieCube copy = state;
            copy.apply(seq);
            unsigned d = depth + mbc_node->depth;
            children.emplace_back(
                new StepNode(copy, seq, this->shared_from_this(), d));
        }

        // Solve the inverse
        CubieCube inv_cc = state.get_inverse();
        auto root_inv = initialize(inv_cc);
        auto step_mbc_inv = solve(root_inv, max_depth);

        for (auto&& mbc_node : step_mbc_inv) {
            Algorithm seq = mbc_node->get_path();

            CubieCube copy = inv_cc;
            copy.apply(seq);
            seq.inv_flag = !this->seq.inv_flag;
            unsigned d = depth + mbc_node->depth;
            children.emplace_back(
                new StepNode{copy, seq, this->shared_from_this(), d});
        }
        return children;
    }
};

template <typename Initializer, typename Solver, typename NextStepper>
auto make_stepper(const Initializer& initialize, const Solver& solve,
                  const NextStepper& next_stepper) {
    return [&initialize, &solve, &next_stepper](
               const std::vector<StepNode::sptr>& prev_step_cc,
               const unsigned move_budget,
               const unsigned breadth) -> std::vector<StepNode::sptr> {
        auto step_cc = std::vector<StepNode::sptr>{};
        step_cc.reserve(breadth);

        for (auto&& step_node : prev_step_cc) {
            unsigned depth = step_node->depth;
            auto children = step_node->expand(initialize, solve,
                                              move_budget - step_node->depth);

            if (step_cc.size() + children.size() > breadth) {
                break;  // Do not expand more nodes if breadth is
                        // reached
            }
            step_cc.insert(step_cc.end(), children.begin(), children.end());
        }

        std::sort(step_cc.begin(), step_cc.end(),
                  [](const StepNode::sptr& a, const StepNode::sptr& b) {
                      return a->depth < b->depth;
                  });

        if constexpr (std::is_invocable_v<NextStepper,
                                          const std::vector<StepNode::sptr>&,
                                          unsigned, unsigned>) {
            return next_stepper(step_cc, move_budget, breadth);
        } else {
            return step_cc;
        }
    };
};

struct STEPFINAL {};

// auto expand_step_three =
//     make_expander(block_solver_F2Lm1::cc_initialize,
//     block_solver_F2Lm1::solve);
// auto expand_step_two =
//     make_expander(block_solver_223::cc_initialize, block_solver_223::solve);
// auto expand_step_one =
//     make_expander(block_solver_222::cc_initialize, block_solver_222::solve);
auto make_step_three = make_stepper(block_solver_F2Lm1::cc_initialize,
                                    block_solver_F2Lm1::solve, STEPFINAL{});
auto make_step_two = make_stepper(block_solver_223::cc_initialize,
                                  block_solver_223::solve, make_step_three);
auto make_step_one = make_stepper(block_solver_222::cc_initialize,
                                  block_solver_222::solve, make_step_two);
