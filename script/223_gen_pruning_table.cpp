#include <iostream>

#include "block_cube.hpp"
#include "pruning_table.hpp"

auto DL_223 = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});

int main() {
    auto b = DL_223;

    b.show();
    std::cout << "Permutation Pruning Table:" << std::endl;
    PermutationPruningTable ppt(b);
    ppt.gen<true>();

    std::cout << "Optimal Pruning Table:" << std::endl;
    OptimalPruningTable opt(b);
    opt.gen<true>();
}