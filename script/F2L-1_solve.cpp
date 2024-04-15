#include <iostream>

#include "option.hpp"
#include "step.hpp"

using Cube = MultiBlockCube<4>;
auto block1 = Block<2, 3>("RouxFB", {DLF, DLB}, {DL, LB, LF});
auto block2 = Block<2, 3>("DB_123", {DLB, DRB}, {DB, RB, LB});
auto block3 = Block<3, 3>("someWeirdThing", {DLF, DLB, DRB}, {DL, LB, DB});
auto block4 = Block<0, 4>("BottomCross", {}, {DF, DR, DB, DL});

auto mover = make_mover(block1, block2, block3, block4);

auto pruner = make_pruner(block1, block2, block3, block4);

std::vector<Algorithm> rotations{
    {},       {x},     {x2},    {x3},     {x, y},  {x2, y}, {x3, y},  {y},
    {x3, z3}, {z3},    {x, z3}, {x2, z3}, {y2},    {y2, x}, {y2, x2}, {y2, x3},
    {y, x3},  {x, z3}, {x, y},  {z},      {y3, z}, {x2, y}, {z, y3},  {y3}};

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto roots = init_roots(scramble, rotations, mover);
    auto solutions = IDAstar<false>(roots, mover.get_apply(),
                                    pruner.template get_estimator<0, 1, 2, 3>(),
                                    get_is_solved<0, 1, 2, 3>(roots[0]->state));

    std::cout << "Optimal solutions to F2L-1" << std::endl;
    for (auto solution : solutions) {
        solution->get_path().show();
    }
}