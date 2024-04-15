#include <tuple>

#include "option.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"

auto block1 = Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB});
auto block2 = Block<1, 2>("DL_F_sq", {DLF}, {DF, LF});
auto block3 = Block<1, 2>("DB_R_sq", {DRB}, {RB, DR});
auto block4 = Block<0, 4>("TopCross", {}, {UF, UR, UB, UL});

auto mover = make_eo_mover(block1, block2, block3, block4);
auto pruner = make_eo_pruner(block1, block2, block3, block4);
auto steps = make_steps({make_step<0, 1>(mover, pruner),
                         make_step<0, 1, 2>(mover, pruner),
                         make_step<0, 1, 2, 3>(mover, pruner),
                         make_step<0, 1, 2, 3, 4>(mover, pruner)});

std::vector<Algorithm> rotations{
    {},       {x},     {x2},    {x3},     {x, y},  {x2, y}, {x3, y},  {y},
    {x3, z3}, {z3},    {x, z3}, {x2, z3}, {y2},    {y2, x}, {y2, x2}, {y2, x3},
    {y, x3},  {x, z3}, {x, y},  {z},      {y3, z}, {x2, y}, {z, y3},  {y3}};

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();
    std::vector<unsigned> splits = {
        get_option("-s0", argc, argv, 10), get_option("-s1", argc, argv, 15),
        get_option("-s2", argc, argv, 19), get_option("-s3", argc, argv, 23)};
    unsigned slackness = get_option("-slackness", argc, argv, 1);

    auto solutions = init_roots(scramble, rotations, mover);
    solutions = multi_phase_search(solutions, steps, splits, slackness);

    solutions.sort_by_depth();
    std::cout << "Corner skeletons" << std::endl;
    for (auto sol : solutions) {
        sol->get_skeleton({"EO - 2x2x2", "2x2x3", "F2L-1", "xC"}).show();
        std::cout << "-------------" << std::endl;
    }
    return 0;
}