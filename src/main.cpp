#include <cassert>
#include <cstring>

#include "123.hpp"
#include "222.hpp"
#include "223.hpp"
#include "F2L-1.hpp"
#include "multistep.hpp"

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    if (strcmp(argv[1], "123") == 0) {
        auto root = block_solver_123::initialize(scramble);

        auto solutions = block_solver_123::solve(root);

        solutions.sort_by_depth();
        solutions.show();
    } else if (strcmp(argv[1], "222") == 0) {
        auto root = block_solver_222::initialize(scramble);

        auto solutions = block_solver_222::solve(root);

        solutions.sort_by_depth();
        solutions.show();
    } else if (strcmp(argv[1], "223") == 0) {
        auto root = block_solver_223::initialize(scramble);

        auto solutions = block_solver_223::solve(root);

        solutions.sort_by_depth();
        solutions.show();
    } else if (strcmp(argv[1], "F2L-1") == 0) {
        auto root = block_solver_F2Lm1::initialize(scramble);

        auto solutions = block_solver_F2Lm1::solve(root);

        solutions.sort_by_depth();
        solutions.show();
    } else if (strcmp(argv[1], "multistep") == 0) {
        CubieCube scramble_cc(scramble);
        auto root = std::make_shared<StepNode>(scramble_cc);
        auto solutions = make_step_one({root}, 15, 500);

        for (auto&& node : solutions) {
            std::cout << "----------------" << std::endl;
            node->get_skeleton({"2x2x2", "2x2x3", "F2L-1"}).show();
        }
    } else {
        std::cout << "Invalid argument: " << argv[1] << std::endl;
    }

    return 0;
}