#include <cassert>
#include <cstring>

#include "123.hpp"
#include "222.hpp"
#include "223.hpp"
#include "F2L-1.hpp"
#include "multistep.hpp"
#include "option.hpp"
#include "two_gen.hpp"

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);

    unsigned slackness = get_option("-s", argc, argv, 0);
    unsigned max_depth = get_option("-M", argc, argv, 15);
    unsigned breadth = get_option("-b", argc, argv, 5000);
    bool linear = find_option("-L", argc, argv);

    if (strcmp(argv[1], "123") == 0) {
        auto root = block_solver_123::initialize(scramble);
        auto solutions = block_solver_123::solve(root, max_depth, slackness);
        solutions.sort_by_depth();
        solutions.show();

        if (linear) {
            auto root_inverse =
                block_solver_123::initialize(scramble.get_inverse());
            auto solutions_inverse =
                block_solver_123::solve(root_inverse, max_depth, slackness);
            solutions_inverse.sort_by_depth();
            solutions_inverse.show(true);
        }
    } else if (strcmp(argv[1], "222") == 0) {
        auto root = block_solver_222::initialize(scramble);
        auto solutions = block_solver_222::solve(root, max_depth, slackness);
        solutions.sort_by_depth();
        solutions.show();

        if (linear) {
            auto root_inverse =
                block_solver_222::initialize(scramble.get_inverse());
            auto solutions_inverse =
                block_solver_222::solve(root_inverse, max_depth, slackness);
            solutions_inverse.sort_by_depth();
            solutions_inverse.show(true);
        }
    } else if (strcmp(argv[1], "223") == 0) {
        auto root = block_solver_223::initialize(scramble);
        auto solutions = block_solver_223::solve(root, max_depth, slackness);
        solutions.sort_by_depth();
        solutions.show();

        if (linear) {
            auto root_inverse =
                block_solver_223::initialize(scramble.get_inverse());
            auto solutions_inverse =
                block_solver_223::solve(root_inverse, max_depth, slackness);
            solutions_inverse.sort_by_depth();
            solutions_inverse.show(true);
        }
    } else if (strcmp(argv[1], "F2L-1") == 0) {
        auto root = block_solver_F2Lm1::initialize(scramble);
        auto solutions = block_solver_F2Lm1::solve(root, max_depth, slackness);
        solutions.sort_by_depth();
        solutions.show();

        if (linear) {
            auto root_inverse =
                block_solver_F2Lm1::initialize(scramble.get_inverse());
            auto solutions_inverse =
                block_solver_F2Lm1::solve(root_inverse, max_depth, slackness);
            solutions_inverse.sort_by_depth();
            solutions_inverse.show(true);
        }
    } else if (strcmp(argv[1], "multistep") == 0) {
        auto solutions = multistep(scramble, max_depth, breadth, slackness);

        for (auto&& node : solutions) {
            std::cout << "----------------" << std::endl;
            node->get_skeleton({"2x2x2", "2x2x3", "F2L-1"}).show();
        }
    } else if (strcmp(argv[1], "two_gen_finish") == 0) {
        unsigned max_depth = get_option("-M", argc, argv, 20);
        auto root = two_gen::initialize(scramble);

        auto solutions = two_gen::solve(root, max_depth, slackness);

        solutions.sort_by_depth();
        solutions.show();

    } else if (strcmp(argv[1], "two_gen_reduction") == 0) {
        auto root = two_gen_reduction::initialize(scramble);
        auto solutions = two_gen_reduction::solve(root, max_depth, slackness);
        solutions.sort_by_depth();
        solutions.show();

        if (linear) {
            auto root_inverse =
                two_gen_reduction::initialize(scramble.get_inverse());
            auto solutions_inverse =
                two_gen_reduction::solve(root_inverse, max_depth, slackness);
            solutions_inverse.sort_by_depth();
            solutions_inverse.show(true);
        }
    } else if (strcmp(argv[1], "two_gen") == 0) {
        unsigned max_depth = get_option("-M", argc, argv, 25);
        two_gen::load_tables();
        two_gen_reduction::load_tables();

        auto root = std::make_shared<StepNode>(scramble);
        auto solutions = reduction({root}, max_depth, breadth, slackness);
        for (auto&& node : solutions) {
            std::cout << "----------------" << std::endl;
            node->get_skeleton({"2-gen Reduction", "Finish"}).show();
        }
    } else {
        std::cout << "Invalid argument: " << argv[1] << std::endl;
    }

    return 0;
}