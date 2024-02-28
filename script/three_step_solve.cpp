#include <tuple>

#include "option.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"

Block<1, 3> block_1("DLB_222", {DLB}, {DL, LB, DB});
Block<1, 2> block_2("DL_F_sq", {DLF}, {DF, LF});
Block<1, 2> block_3("DB_R_sq", {DRB}, {RB, DR});

auto mover = Mover(new BlockMoveTable(block_1), new BlockMoveTable(block_2),
                   new BlockMoveTable(block_3));

auto pruner = Pruner(load_table_ptr(Strategy::Optimal(block_1)),
                     load_table_ptr(Strategy::Optimal(block_2)),
                     load_table_ptr(Strategy::Optimal(block_3)));

std::vector<Algorithm> rotations{{},   {x},     {x2},     {x3},
                                 {y2}, {y2, x}, {y2, x2}, {y2, x3}};

using namespace Method;

int main(int argc, const char* argv[]) {
    std::array<unsigned, 3> splits_move_counts{
        get_option<unsigned>("-s0", argc, argv),
        get_option<unsigned>("-s1", argc, argv),
        get_option<unsigned>("-s2", argc, argv)};
    auto scramble = Algorithm::make_from_str(argv[argc - 1]);
    scramble.show();
    MultiBlockCube<3> cube;
    mover.apply(scramble, cube);

    auto roots = init_roots(scramble, rotations, mover);

    // Step 1 : 2x2x2
    // StepSolutions step1_solutions;
    // for (auto root : roots) {
    //     auto tmp = expand(root, mover, pruner, splits_move_counts);
    //     step1_solutions.insert(step1_solutions.end(), tmp.begin(),
    //     tmp.end());
    // }

    // // Step 2 : 2x2x3
    // StepSolutions step2_solutions;
    // for (auto node : step1_solutions) {
    //     auto tmp = expand(node, mover, pruner, splits_move_counts);
    //     step2_solutions.insert(step2_solutions.end(), tmp.begin(),
    //     tmp.end());
    // }

    // // Step 3 : F2L-1
    // StepSolutions step3_solutions;
    // for (auto node : step2_solutions) {
    //     auto tmp = expand(node, mover, pruner, splits_move_counts);
    //     step3_solutions.insert(step3_solutions.end(), tmp.begin(),
    //     tmp.end());
    // }
    // std::cout << "Three step F2L-1 solutions" << std::endl;
    // for (auto solution : step3_solutions) {
    //     show(get_skeleton(solution));
    //     std::cout << std::endl;
    // }
    return 0;
}