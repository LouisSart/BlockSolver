#include <iostream>

#include "block_cube.hpp"
#include "pruning_table.hpp"
#include "solve.hpp"

auto LF_column = Block<2, 1>("LF_column", {ULF, DLB}, {LF});
auto DLB_222 = Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB});
auto DL_223 = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});
auto DLB_F2Lm1 =
    Block<3, 7>("DLB_F2L-1", {DLF, DLB, DRB}, {LF, LB, DF, DB, DL, RB, DR});

int main() {
    Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                        U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});

    auto b = DLB_F2Lm1;
    auto strat = Strategy::Split(b);
    auto solutions = ask_if_generate_and_solve(scramble, strat);
    show(solutions);
    assert(solutions.size() == 1);
}