#include <iostream>

#include "block_cube.hpp"
#include "pruning_table.hpp"

auto LF_column = Block<2, 1>("LF_column", {ULF, DLB}, {LF});
auto DLB_222 = Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB});
auto DL_223 = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});
auto DLB_F2Lm1 =
    Block<3, 7>("DLB_F2L-1", {DLF, DLB, DRB}, {LF, LB, DF, DB, DL, RB, DR});

int main() {
    auto b = DLB_F2Lm1;
    PermutationPruningTable table(b);
    table.gen<true>();
    // unfortunately this is still too much for my PC, RAM gets overflowed
}