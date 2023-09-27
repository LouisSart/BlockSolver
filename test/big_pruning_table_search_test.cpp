#include "pruning_table.hpp"
#include "search.hpp"


int main(){
    Block<2, 5> b("DL_223", {4, 7}, {4, 7, 8, 10, 11});
    OptimalPruningTable p_table(b);
    BlockMoveTable m_table(b);
    CoordinateBlockCube cbc;
    Algorithm scramble({R3, U3, F, D2, R2, F3, L2, D2, F3, L, U3, B, U3, D3, F2, B2, L2, D, F2, U2, D, R3, U3, F});
    m_table.apply(scramble, cbc);

    Node<CoordinateBlockCube> root(cbc, 0);
    auto solutions = depth_first_search(
        root,
        m_table,
        p_table,
        9
    );
    for (auto&& s : solutions) {
        s.show();
    }
    return 0.;
}