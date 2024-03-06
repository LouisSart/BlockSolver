#include <iostream>

#include "option.hpp"
#include "step.hpp"

auto method =
    make_method(Block<2, 3>("RouxFB", {DLF, DLB}, {DL, LB, LF}),
                Block<2, 3>("DB_123", {DLB, DRB}, {DB, RB, LB}),
                Block<3, 3>("someWeirdThing", {DLF, DLB, DRB}, {DL, LB, DB}),
                Block<0, 4>("BottomCross", {}, {DF, DR, DB, DL}));

std::vector<Algorithm> rotations{
    {},       {x},     {x2},    {x3},     {x, y},  {x2, y}, {x3, y},  {y},
    {x3, z3}, {z3},    {x, z3}, {x2, z3}, {y2},    {y2, x}, {y2, x2}, {y2, x3},
    {y, x3},  {x, z3}, {x, y},  {z},      {y3, z}, {x2, y}, {z, y3},  {y3}};

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto solutions = method.init_roots(scramble, rotations);
    solutions = method.make_step<0, 1, 2, 3>(solutions, 20, OPT_ONLY);

    std::cout << "Optimal solutions to F2L-1" << std::endl;
    for (auto solution : solutions) {
        show(solution->get_skeleton());
        std::cout << std::endl;
    }
}