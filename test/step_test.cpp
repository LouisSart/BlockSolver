#include "step.hpp"

#include "pruning_table.hpp"

Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                    U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});

Strategy::Optimal strat_1(Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB}));

void test_step_node() {
    MultiBlockCube<3> state;
    state.show();
}

int main() {
    test_step_node();
    return 0;
}