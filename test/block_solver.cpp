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
}

int main (){
    test_partial();
    test_block_solves();
    Block<1, 3> b222({DLB}, {DL, DB, LB}, "222");
    Block<2, 4> extension ({DLF, DRB}, {LF, DF, DR, RB});
    Block<1, 3> finish ({DRF}, {RF, LF, RB});

    BlockSolver b222_solver{b222};
    BlockSolver xxcross_solver{b222, extension};
    BlockSolver f2l_solver{b222, extension, finish};

    auto root = make_root(CubieCube("R' U' F L2 U2 L2 B' U2 F2 L2 B L2 D2 L2 D2 F U' R' B' R' F' U2 R' B R' U' F"));

    auto multistep = make_multistep_solver(b222_solver, xxcross_solver, f2l_solver);
    for (auto sol : multistep.solve({root})) {
        sol->get_path<Move>().show();
    }
	return 0;
}