#include "../src/block.hpp"

using namespace Cube333;

void test_partial() {
    CubieCube cc;
    Block<0, 4> cross({}, {DF, DL, DB, DR});
    Block<1, 3> b222 ({DLB}, {DL, DB, LB});
    Block<4, 0> top_corners ({ULF, URF, URB, ULB}, {});

    auto check_index = [](const auto& b) -> void {
        CubieCube cc;
        for (unsigned c = 0; c < b.CARD; ++c) {
            b.from_index(c, cc);
            assert(b.index(cc) == c);
        }
    };

    check_index(cross);
    check_index(b222);
    check_index(top_corners);
}

void test_block_solves() {
    auto root = make_root(CubieCube("R' U' F L U D F2 D' F B U F' R2 F2 U2 F2 D2 R' F2 L2 D2 B2 L R' U' F"));
    Block<0, 4> cross({}, {DF, DL, DB, DR}, "cross");
    BlockSolver cross_solver{cross};
    assert(cross_solver.solve({root}).size() == 12);

    root = make_root(CubieCube("B' U2 F R' U' F R2 L2 D2 B D2 F2 R2 L2 U2 B D2 F' L' D2 F' U' F U2 B U2 R' L2 U' F F R' D' R F2"));
    Block<1, 3> b222({DLB}, {DL, DB, LB}, "222");
    Block<1, 2> b223_ext ({DLF}, {LF, DF}, "222_to_223");
    Block<1, 2> xxcross_ext ({DRB}, {DR, RB}, "223_to_f2l-1");
    Block<0, 4> top_cross ({}, {UF, UL, UR, UB}, "f2l-1_to_solved_edges");
    BlockSolver b222_solver{b222};
    BlockSolver b223_solver{b222, b223_ext};
    BlockSolver xxcross_solver{b222, b223_ext, xxcross_ext};
    BlockSolver edge_solver{b222, b223_ext, xxcross_ext, top_cross};
    auto multistep = make_multistep_solver(b222_solver, b223_solver, xxcross_solver, edge_solver);
    multistep.solve({root});
}

int main (){
    test_partial();
    test_block_solves();
	return 0;
}