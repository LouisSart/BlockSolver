#include "solve.hpp"

template <bool verbose = false, typename Strategy, typename Solutions>
auto step(const Algorithm& scramble, const Strategy& strat,
          const Solutions& prev_step_solutions, const unsigned& max_step_length,
          const unsigned& max_search_depth) {
    std::vector<Algorithm> solutions;
    auto p_table = PruningTable(strat);
    for (auto solution : prev_step_solutions) {
        Algorithm setup = scramble;
        setup.append(solution);
        unsigned search_depth =
            (max_step_length - solution.size() < max_search_depth)
                ? max_search_depth
                : max_step_length - solution.size();
        auto continuations =
            solve<verbose>(setup, p_table, strat, search_depth);
        for (auto continuation : continuations) {
            auto combination = solution;
            combination.append(continuation);
            solutions.push_back(combination);
        }
    }
    return solutions;
}

int main() {
    Algorithm scramble({R3, U3, F, L2, B2, U3, D3, B,  R3, F2, D,  R2, F,
                        L,  B2, U, L2, F2, U3, R2, U2, L2, U2, R3, U3, F});
    scramble.show();

    Strategy::Optimal strat_1(Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB}));

    auto solutions = solve<false>(scramble, strat_1, 6);
    std::cout << "\nSolutions for step 1 (DLB 2x2x2):" << std::endl;
    show(solutions);

    Strategy::Permutation strat_2(
        Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL}));
    solutions = step(scramble, strat_2, solutions, 11, 6);
    std::cout << "\nSolutions for step 2 (DL 2x2x3):" << std::endl;
    show(solutions);

    Strategy::Permutation strat_3(Block<3, 7>("DLB_F2L-1", {DLF, DLB, DRB},
                                              {LF, LB, DF, DB, DL, RB, DR}));
    solutions = step(scramble, strat_3, solutions, 17, 6);
    std::cout << "\nSolutions for step 3 (DLB F2L-1):" << std::endl;
    show(solutions);

    return 0;
}