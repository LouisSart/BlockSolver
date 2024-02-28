#include <tuple>

#include "option.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"

constexpr unsigned _NBLOCKS = 3;

Block<1, 3> block_1("DLB_222", {DLB}, {DL, LB, DB});
Block<1, 2> block_2("DL_F_sq", {DLF}, {DF, LF});
Block<1, 2> block_3("DB_R_sq", {DRB}, {RB, DR});

auto method = make_method(block_1, block_2, block_3);

std::vector<Algorithm> rotations{{},   {x},     {x2},     {x3},
                                 {y2}, {y2, x}, {y2, x2}, {y2, x3}};

using StepSolutions = Solutions<StepNode<MultiBlockCube<_NBLOCKS>>::sptr>;

int main(int argc, const char* argv[]) {
    std::array<unsigned, 3> splits_move_counts{
        get_option<unsigned>("-s0", argc, argv),
        get_option<unsigned>("-s1", argc, argv),
        get_option<unsigned>("-s2", argc, argv)};
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto roots = method.init_roots(scramble, rotations);

    // Step 1 : 2x2x2
    StepSolutions step1_solutions;
    for (auto root : roots) {
        auto tmp = method.make_step<0>(root, splits_move_counts[0]);
        step1_solutions.insert(step1_solutions.end(), tmp.begin(), tmp.end());
    }

    // // Step 2 : 2x2x3
    StepSolutions step2_solutions;
    for (auto node : step1_solutions) {
        auto tmp = method.make_step<0, 1>(node, splits_move_counts[1]);
        step2_solutions.insert(step2_solutions.end(), tmp.begin(), tmp.end());
    }

    // Step 3 : F2L-1
    StepSolutions step3_solutions;
    for (auto node : step2_solutions) {
        auto tmp = method.make_step<0, 1, 2>(node, splits_move_counts[2]);
        step3_solutions.insert(step3_solutions.end(), tmp.begin(), tmp.end());
    }

    std::cout << "Three step F2L-1 solutions" << std::endl;
    for (auto solution : step3_solutions) {
        show(solution->get_skeleton());
        std::cout << std::endl;
    }
    return 0;
}