#include "option.hpp"
#include "step.hpp"

using Cube = MultiBlockCube<3>;
auto block1 = Block<2, 3>("RouxFB", {DLF, DLB}, {DL, LB, LF});
auto block2 = Block<1, 2>("DL_F_sq", {DLF}, {DF, LF});
auto block3 = Block<1, 2>("DL_B_sq", {DLB}, {DB, LB});

auto mover = make_mover(block1, block2, block3);

auto pruner = make_pruner(block1, block2, block3);

std::vector<Algorithm> rotations{{},      {x},    {x2},     {x3},
                                 {y},     {y2},   {y3},     {z2},
                                 {x3, y}, {x, y}, {x3, y2}, {x, y2}};

int main(int argc, const char* argv[]) {
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto roots = init_roots<Cube>(scramble, rotations, mover);
    auto solutions = IDAstar<false>(roots, mover.get_apply(),
                                    pruner.template get_estimator<0, 1, 2>(),
                                    get_is_solved<0, 1, 2>(roots[0]->state));

    std::cout << "Solutions to 2x2x3" << std::endl;
    for (auto solution : solutions) {
        solution->get_path().show();
    }
    return 0.;
}