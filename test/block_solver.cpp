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

int main (){
    test_partial();
    CubieCube cc("R' U' F L U D F2 D' F B U F' R2 F2 U2 F2 D2 R' F2 L2 D2 B2 L R' U' F");
    BlockSolver<0, 4> cross_solver({}, {DF, DL, DB, DR});
    assert(cross_solver.solve(cc).size() == 12);

    BlockSolver<1, 3> b222_solver({DLB}, {DL, DB, LB});
    b222_solver.solve(cc);
    BlockSolver<2, 4> extension_solver ({DLF, DRB}, {LF, DF, DR, RB});
    extension_solver.solve(cc).show<Move>();

	return 0;
}