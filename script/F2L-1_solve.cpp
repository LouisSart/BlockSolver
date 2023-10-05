#include <iostream>

#include "block_cube.hpp"
#include "move_table.hpp"
#include "pruning_table.hpp"

int main() {
    auto DLB_222 = Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB});
    auto DL_223 = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});
    auto DLB_F2Lm1 =
        Block<3, 7>("DLB_F2L-1", {DLF, DLB, DRB}, {LF, LB, DF, DB, DL, RB, DR});
    std::cout << "Generating permutation pruning table for Block:" << std::endl;
    DLB_F2Lm1.show();
    PermutationPruningTable table(DLB_F2Lm1);
}