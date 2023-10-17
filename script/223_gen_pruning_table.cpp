#include <iostream>

#include "block_cube.hpp"
#include "pruning_table.hpp"

auto DL_223 = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});

int main() {
    auto b = DL_223;

    b.show();
    std::cout << "Permutation Pruning Table:" << std::endl;
    auto ppt = Strategy::Permutation(b).gen_table<true>();

    std::cout << "Optimal Pruning Table:" << std::endl;
    auto opt = Strategy::Optimal(b).gen_table<true>();
}