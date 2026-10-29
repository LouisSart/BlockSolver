#pragma once
#include "222.hpp"
#include "223.hpp"
#include "F2L-1.hpp"
#include "step_node.hpp"

auto make_step_three = make_stepper(block_solver_F2Lm1::cc_initialize,
                                    block_solver_F2Lm1::solve, STEPFINAL{});
auto make_step_two = make_stepper(block_solver_223::cc_initialize,
                                  block_solver_223::solve, make_step_three);
auto make_step_one = make_stepper(block_solver_222::cc_initialize,
                                  block_solver_222::solve, make_step_two);

auto multistep(const CubieCube& scramble, const unsigned max_depth,
               const unsigned breadth, const unsigned slackness) {
    auto root = std::make_shared<StepNode>(scramble);
    auto move_budget = 0;
    std::vector<StepNode::sptr> solutions;

    while (solutions.size() == 0 && move_budget <= max_depth) {
        solutions = make_step_one({root}, move_budget, breadth, slackness);
        move_budget++;
    }
    return solutions;
}
