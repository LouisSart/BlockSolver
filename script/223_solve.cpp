#include "pruning_table.hpp"
#include "solve.hpp"

int main() {
    Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                        U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
    scramble.show();
    auto DL_223 = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});

    std::cout << "2x2x3 Solve with optimal pruning table" << std::endl;
    OptimalPruningTable op_table(DL_223);
    auto solutions = solve(scramble, op_table);
    show(solutions);
    assert(solutions.size() == 10);

    std::cout << "2x2x3 Solve with permutation pruning table" << std::endl;
    PermutationPruningTable pe_table(DL_223);
    solutions = solve(scramble, pe_table);
    show(solutions);
    assert(solutions.size() == 10);
    return 0.;
}