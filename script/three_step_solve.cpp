#include "solve.hpp"
#include "step.hpp"

Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                    U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});

Strategy::Optimal strat_1(Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB}));
std::vector<Algorithm> setups_1{{},   {y},     {y2},     {y3},
                                {z2}, {z2, y}, {z2, y2}, {z2, y3}};

Strategy::Optimal strat_2(Block<2, 5>("DL_223", {DLF, DLB},
                                      {LF, LB, DF, DB, DL}));
std::vector<Algorithm> setups_2{{}, {x, y}, {y3, x3}};

Strategy::Permutation strat_3(Block<3, 7>("DLB_F2L-1", {DLF, DLB, DRB},
                                          {LF, LB, DF, DB, DL, RB, DR}));
std::vector<Algorithm> setups_3{{}, {z3, y2}};

int main() {
    scramble.show();

    std::cout << "Step 1 (2x2x2)" << std::endl;
    Step step_1(scramble, strat_1, setups_1, 5, 5);
    StepNode root(scramble);
    auto step_1_solutions = step_1.expand(StepSolutions({root}));
    // step_1_solutions.show();

    std::cout << "Step 2 (2x2x3)" << std::endl;
    Step step_2(scramble, strat_2, setups_2, 5, 10);
    auto step_2_solutions = step_2.expand(step_1_solutions);
    step_2_solutions.show();

    std::cout << "Step 3 (F2L-1)" << std::endl;
    Step step_3(scramble, strat_3, setups_3, 7, 15);
    auto step_3_solutions = step_3.expand(step_2_solutions);
    step_3_solutions.show();
    return 0;
}