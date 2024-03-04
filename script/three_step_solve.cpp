#include <tuple>

#include "option.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"

auto method = make_method(Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB}),
                          Block<1, 2>("DL_F_sq", {DLF}, {DF, LF}),
                          Block<1, 2>("DB_R_sq", {DRB}, {RB, DR}),
                          Block<0, 4>("TopCross", {}, {UF, UR, UB, UL}));

std::vector<Algorithm> rotations{
    {},       {x},     {x2},    {x3},     {x, y},  {x2, y}, {x3, y},  {y},
    {x3, z3}, {z3},    {x, z3}, {x2, z3}, {y2},    {y2, x}, {y2, x2}, {y2, x3},
    {y, x3},  {x, z3}, {x, y},  {z},      {y3, z}, {x2, y}, {z, y3},  {y3}};

int main(int argc, const char* argv[]) {
    std::array<unsigned, 4> splits_move_counts{
        get_option<unsigned>("-s0", argc, argv),
        get_option<unsigned>("-s1", argc, argv),
        get_option<unsigned>("-s2", argc, argv)};
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto solutions = method.init_roots(scramble, rotations);

    // Step 1 : 2x2x2
    solutions = method.make_step<0>(solutions, splits_move_counts[0]);
    // Step 1 : F2L-1
    solutions = method.make_step<0, 1, 2>(solutions, splits_move_counts[1]);
    // Step 1 : L5C skeleton
    solutions = method.make_step<0, 1, 2, 3>(solutions, splits_move_counts[2]);

    solutions.sort_by_depth();
    std::cout << "Three step corner skeletons" << std::endl;
    for (auto solution : solutions) {
        show(solution->get_skeleton());
        std::cout << std::endl;
    }
    return 0;
}