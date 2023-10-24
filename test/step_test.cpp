#include "step.hpp"

#include "algorithm.hpp"
#include "cubie_cube.hpp"

int main() {
    Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                        U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});

    Strategy::Optimal strat_1(Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB}));
    std::vector<Algorithm> setups_1{{},   {y},     {y2},     {y3},
                                    {z2}, {z2, y}, {z2, y2}, {z2, y3}};
    Step step_1(scramble, strat_1, setups_1, 7, 7);
    StepNode root(scramble);
    std::vector<StepNode> depth_1 = step_1.setup_expand(root);
    CubieCube cc(scramble);
    assert(cc == root.state);
    for (const auto& node : depth_1) {
        CubieCube ccc, stp;
        ccc.apply(node.setups.back());
        ccc.apply(node.state);
        stp.apply(node.setups.back());
        ccc.apply(stp.get_inverse());
        assert(ccc == root.state);
        assert(ccc == cc);
    }
    return 0;
}