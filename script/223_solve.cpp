#include "option.hpp"
#include "step.hpp"

auto block1 = Block<2, 3>("RouxFB", {DLF, DLB}, {DL, LB, LF});
auto block2 = Block<1, 2>("DL_F_sq", {DLF}, {DF, LF});
auto block3 = Block<1, 2>("DL_B_sq", {DLB}, {DB, LB});

auto mover = make_mover(block1, block2, block3);

auto pruner = make_pruner(block1, block2, block3);

std::vector<Algorithm> rotations{
    {},      {x},     {x2}, {x3},   {y},     {y, x},
    {y, x2}, {y, x3}, {z},  {z, x}, {z, x2}, {z, x3},
};

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto roots = init_roots(scramble, rotations, mover);
    // auto solutions = method.make_step<0, 1, 2>(roots, 20, OPT_ONLY);

    // std::cout << "Solutions to 2x2x3" << std::endl;
    // for (auto solution : solutions) {
    //     show(solution->get_skeleton());
    //     std::cout << std::endl;
    // }
    return 0.;
}