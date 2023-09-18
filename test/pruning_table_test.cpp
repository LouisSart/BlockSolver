#include "pruning_table.hpp"


void one_corner_and_one_edge(){
    std::cout << "Solution lengths for one corner (UFL) and one edge (UF): " << std::endl;
    OptimalPruningTable OneC_OneE_table(Block<1, 1>("OneCornerAndOneEdge", {0}, {0}));
    OptimalPruningTable table_reloaded(Block<1, 1>("OneCornerAndOneEdge", {0}, {0}));

    for (int k=0; k<OneC_OneE_table.table_size; ++k){
        assert(OneC_OneE_table.table[k] == table_reloaded.table[k]);
    }
}

void DLB_2x2x2(){
    std::cout << "Solution lengths for the DLB 2x2x2: " << std::endl;
    OptimalPruningTable DLB_222(Block<1, 3>("DLB_222", {7}, {7, 10, 11}));
    OptimalPruningTable table_reloaded(Block<1, 3>("DLB_222", {7}, {7, 10, 11}));

    for (int k=0; k<DLB_222.table_size; ++k){
        assert(DLB_222.table[k] == table_reloaded.table[k]);
    }
}
void roux_fb(){
    std::cout << "Solution lengths for Roux First Block :" << std::endl;
    OptimalPruningTable Roux_FB_table(Block<2, 3>("RouxFirstBlock", {4, 7}, {4, 7, 11}));
}

void DL_2x2x3(){
    // Unable to run this on my computer. 5 edges and 2 corners is too big
    std::cout << "Solution lengths for DL 2x2x3 :" << std::endl;
    OptimalPruningTable DL_223(Block<2, 5>("DL_223", {4, 7}, {4, 7, 8, 10, 11}));
}

void top_layer(){
    // Unable to run this on my computer. 4 edges and 4 corners is too big
    std::cout << "Solution lengths for the Top Layer :" << std::endl;
    OptimalPruningTable TopLayer_table(Block<4, 4>("TopLayer", {0, 1, 2, 3}, {0, 1, 2, 3}));
}

void test_api(){
    CubieCube cc;
    
    OptimalPruningTable DL_223(Block<2, 3>("DL_223", {4, 7}, {4, 7, 8, 10, 11}));

}

int main(){
    one_corner_and_one_edge();
    DLB_2x2x2();
    roux_fb();
    return 0;
}