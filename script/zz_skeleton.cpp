#include "option.hpp"
#include "pruning_table.hpp"
#include "search.hpp"
#include "step.hpp"

auto block1 = Block<0, 2>("ZZ_line", {}, {DF, DB});
auto block2 = Block<2, 3>("RouxFB", {DLF, DLB}, {DL, LB, LF});
auto block3 = Block<2, 3>("RouxSB", {DRF, DRB}, {DR, RB, RF});
auto block4 = Block<0, 4>("TopCross", {}, {UF, UR, UB, UL});

auto mover = Mover(new EOMoveTable(), new BlockMoveTable(block1),
                   new BlockMoveTable(block2), new BlockMoveTable(block3),
                   new BlockMoveTable(block4));
auto pruner = Pruner(load_table_ptr(Strategy::OptimalEO()),
                     load_table_ptr(Strategy::Optimal(block1)),
                     load_table_ptr(Strategy::Optimal(block2)),
                     load_table_ptr(Strategy::Optimal(block3)),
                     load_table_ptr(Strategy::Optimal(block4)));
auto method = Method(mover, pruner);

std::vector<Algorithm> rotations{
    {},       {z},        {z2},        {z3},        {y2},        {z, y2},
    {z2, y2}, {z3, y2},   {y},         {y, z},      {y, z2},     {y, z3},
    {y3},     {y, z, y2}, {y, z2, y2}, {y, z3, y2}, {x},         {x, z},
    {x, z2},  {x, z3},    {x, y2},     {x, z, y2},  {x, z2, y2}, {x, z3, y2}};

int main(int argc, const char* argv[]) {
    std::array<unsigned, 4> splits_move_counts{
        get_option<unsigned>("-s0", argc, argv),
        get_option<unsigned>("-s1", argc, argv),
        get_option<unsigned>("-s2", argc, argv),
        get_option<unsigned>("-s3", argc, argv)};
    auto scramble = Algorithm(argv[argc - 1]);
    scramble.show();

    auto solutions = method.init_roots(scramble, rotations);

    // Step 1 : EO Line
    solutions = method.make_step<0, 1>(solutions, splits_move_counts[0]);
    // Step 1 : RouxFB
    solutions = method.make_step<0, 1, 2>(solutions, splits_move_counts[1]);
    // Step 2 : F2L
    solutions = method.make_step<0, 1, 2, 3>(solutions, splits_move_counts[2]);
    // Step 2 : Corner skeleton
    solutions =
        method.make_step<0, 1, 2, 3, 4>(solutions, splits_move_counts[3]);

    solutions.sort_by_depth();
    std::cout << "ZZ skeleton" << std::endl;
    for (auto solution : solutions) {
        show(solution->get_skeleton());
        std::cout << std::endl;
    }
}