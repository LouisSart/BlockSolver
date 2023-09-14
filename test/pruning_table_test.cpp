#include "pruning_table.hpp"

int main(){
    std::cout << "Solution lengths for one corner (UFL) and one edge (UF): " << std::endl;
    OptimalPruningTable OneC_OneE_table(Block<1, 1>("OneCornerAndOneEdge", {0}, {0}));
    std::cout << "Solution lengths for the DLB 2x2x2: " << std::endl;
    OptimalPruningTable DLB_222(Block<1, 3>("DLB_222", {7}, {7, 10, 11}));
    // std::cout << "Solution lengths for Roux First Block :" << std::endl;
    // OptimalPruningTable Roux_FB_table(Block<2, 3>("RouxFirstBlock", {4, 7}, {4, 7, 11}));
    // std::cout << "Solution lengths for the Top Layer :" << std::endl;
    // OptimalPruningTable TopLayer_table(Block<4, 4>("TopLayer", {0, 1, 2, 3}, {0, 1, 2, 3}));
    return 0;
}