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

    assert(solvers::three_step_l3c_solver.solve({root})[0]->depth == 22);
}

int main (){
    test_partial();
    test_block_solves();
	return 0;
}