#include "option.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"

auto block1 = Block<0, 2>("ZZ_line", {}, {DF, DB});
auto block2 = Block<2, 3>("RouxFB", {DLF, DLB}, {DL, LB, LF});
auto block3 = Block<2, 3>("RouxSB", {DRF, DRB}, {DR, RB, RF});
// auto block4 = Block<0, 4>("TopCross", {}, {UF, UR, UB, UL});

auto mover = make_eo_mover(block1, block2, block3);
auto pruner = make_eo_pruner(block1, block2, block3);
auto steps = make_steps({make_step<0, 1>(mover, pruner),
                         make_step<0, 1, 2>(mover, pruner),
                         make_step<0, 1, 2, 3>(mover, pruner)});

std::vector<Algorithm> rotations{
    {},       {z},        {z2},        {z3},        {y2},        {z, y2},
    {z2, y2}, {z3, y2},   {y},         {y, z},      {y, z2},     {y, z3},
    {y3},     {y, z, y2}, {y, z2, y2}, {y, z3, y2}, {x},         {x, z},
    {x, z2},  {x, z3},    {x, y2},     {x, z, y2},  {x, z2, y2}, {x, z3, y2}};

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();
    std::vector<unsigned> splits{get_option("-s0", argc, argv, 7),
                                 get_option("-s1", argc, argv, 13),
                                 get_option("-s2", argc, argv, 20)};
    unsigned slackness = get_option("-slackness", argc, argv, 1);

    auto solutions = init_roots(scramble, rotations, mover);
    solutions = multi_phase_search(solutions, steps, splits, slackness);

    solutions.sort_by_depth();
    std::cout << "ZZ skeletons" << std::endl;
    for (auto sol : solutions) {
        sol->get_skeleton({"EO - line", "2x2x3", "F2L"}).show();
        std::cout << "-------------" << std::endl;
    }
}