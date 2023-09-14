#include "pruning_table.hpp"

int main(){
    // OptimalPruningTable table(Block<4, 4>("TopLayer", {0, 1, 2, 3}, {0, 1, 2, 3}));
    OptimalPruningTable table(Block<1, 1>("OneCornerAndOneEdge", {0}, {0}));
    return 0;
}