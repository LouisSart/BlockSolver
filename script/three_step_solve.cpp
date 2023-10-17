#include "solve.hpp"

auto step_1(const Algorithm& scramble) {
    Block<1, 3> b("DLB_222", {DLB}, {DL, LB, DB});

    return solve<false>(scramble, Strategy::Optimal(b));
}

auto step_2(const Algorithm& scramble, const auto& step_1_solutions) {
    auto b = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});
    std::vector<Algorithm> step_2_solutions;

    for (auto solution : step_1_solutions) {
        Algorithm setup = scramble;
        setup.append(solution);
        auto continuations = solve<false>(setup, Strategy::Permutation(b));
        for (auto continuation : continuations) {
            auto combination = solution;
            combination.append(continuation);
            step_2_solutions.push_back(combination);
        }
    }
    return step_2_solutions;
}

auto step_3(const Algorithm& scramble, const auto& step_2_solutions) {
    auto b =
        Block<3, 7>("DLB_F2L-1", {DLF, DLB, DRB}, {LF, LB, DF, DB, DL, RB, DR});
    std::vector<Algorithm> step_3_solutions;

    for (auto solution : step_2_solutions) {
        Algorithm setup = scramble;
        setup.append(solution);
        auto continuations = solve<false>(setup, Strategy::Permutation(b));
        for (auto continuation : continuations) {
            auto combination = solution;
            combination.append(continuation);
            step_3_solutions.push_back(combination);
        }
    }
    return step_3_solutions;
}

int main() {
    Algorithm scramble({R3, U3, F, L2, B2, U3, D3, B,  R3, F2, D,  R2, F,
                        L,  B2, U, L2, F2, U3, R2, U2, L2, U2, R3, U3, F});
    scramble.show();

    auto solutions = step_1(scramble);
    std::cout << "\nSolutions for step 1 (DLB 2x2x2):" << std::endl;
    show(solutions);

    solutions = step_2(scramble, solutions);
    std::cout << "\nSolutions for step 2 (DL 2x2x3):" << std::endl;
    show(solutions);

    solutions = step_3(scramble, solutions);
    std::cout << "\nSolutions for step 3 (DLB F2L-1):" << std::endl;
    show(solutions);

    return 0;
}