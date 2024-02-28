#include <tuple>

#include "option.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"

Block<1, 3> block_1("DLB_222", {DLB}, {DL, LB, DB});
Block<1, 2> block_2("DL_F_sq", {DLF}, {DF, LF});
Block<1, 2> block_3("DB_R_sq", {DRB}, {RB, DR});

auto method = make_method(block_1, block_2, block_3);

std::vector<Algorithm> rotations{{},   {x},     {x2},     {x3},
                                 {y2}, {y2, x}, {y2, x2}, {y2, x3}};

int main(int argc, const char* argv[]) {
    std::array<unsigned, 3> splits_move_counts{
        get_option<unsigned>("-s0", argc, argv),
        get_option<unsigned>("-s1", argc, argv),
        get_option<unsigned>("-s2", argc, argv)};
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto solutions = method.init_roots(scramble, rotations);

    // Step 1 : 2x2x2
    solutions = method.make_step<0>(solutions, splits_move_counts[0]);
    // Step 1 : 2x2x3
    solutions = method.make_step<0, 1>(solutions, splits_move_counts[1]);
    // Step 1 : F2L-1
    solutions = method.make_step<0, 1, 2>(solutions, splits_move_counts[2]);

    std::cout << "Three step F2L-1 solutions" << std::endl;
    for (auto solution : solutions) {
        show(solution->get_skeleton());
        std::cout << std::endl;
    }
    return 0;
}