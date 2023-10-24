#include "step.hpp"

#include "algorithm.hpp"
#include "cubie_cube.hpp"
#include "solve.hpp"

Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                    U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});

Strategy::Optimal strat_1(Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB}));
std::vector<Algorithm> setups_1{{},   {y},     {y2},     {y3},
                                {z2}, {z2, y}, {z2, y2}, {z2, y3}};

void test_step_node() {
    StepNode root(scramble);
    assert(CubieCube(scramble) == root.state);

    for (const auto& setup : setups_1) {
        StepNode node(root, setup);
        CubieCube ccc, stp;
        ccc.apply(node.setups.back());
        ccc.apply(node.state);
        stp.apply(node.setups.back());
        ccc.apply(stp.get_inverse());
        assert(ccc == root.state);
        assert(ccc == CubieCube(scramble));
    }

    BlockCube bc(strat_1.block);
    scramble.show();
    auto cbc1 = bc.to_coordinate_block_cube(CubieCube(scramble));

    CoordinateBlockCube cbc2;
    BlockMoveTable m_table(strat_1.block);
    m_table.apply(scramble, cbc2);
    assert(cbc1 == cbc2);
    // StepNode node(root, solution, 4);
    // assert(bc.to_coordinate_block_cube(node.state).is_solved());
}

int main() {
    test_step_node();
    return 0;
}