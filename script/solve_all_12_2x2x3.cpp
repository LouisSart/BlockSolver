#include <vector>

#include "algorithm.hpp"
#include "cubie_cube.hpp"
#include "move_table.hpp"
#include "pruning_table.hpp"
#include "solve.hpp"

void symmetries_2x2x3() {
    Block<2, 5> b("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});
    BlockCube bc(b);
    BlockMoveTable m_table(b);
    Strategy::Split strat(b);
    auto p_table = strat.load_table();
    CoordinateBlockCube cbc;
    std::cout << "Solving the DL 2x2x3 on Wen's WR scramble: " << std::endl;
    Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                        U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
    CubieCube symmetry;
    for (int s1 = 0; s1 < 3; ++s1) {
        symmetry.apply(S_URF);
        for (int s2 = 0; s2 < 2; ++s2) {
            symmetry.apply(z2);
            for (int s3 = 0; s3 < 2; ++s3) {
                symmetry.RL_mirror();
                std::cout << std::endl;
                scramble.show();
                std::cout << "S_URF: " << (s1 + 1) % 3
                          << " z2: " << (s2 + 1) % 2 << " LR: " << (s3 + 1) % 2
                          << std::endl;
                CubieCube scrambled_state;
                scrambled_state.apply(symmetry.get_inverse());
                scrambled_state.apply(scramble);
                scrambled_state.apply(symmetry);
                cbc = bc.to_coordinate_block_cube(scrambled_state);
                auto solutions = solve(cbc, m_table, p_table);
                show(solutions);
            }
        }
    }
}

int main() {
    symmetries_2x2x3();
    return 0.;
}