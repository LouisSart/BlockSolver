#include "step.hpp"

void mover_first_test() {
    Block<1, 0> block_1("ULF corner", {ULF}, {});
    Block<0, 1> block_2("UF edge", {}, {UF});
    auto mover =
        Mover{new BlockMoveTable(block_1), new BlockMoveTable(block_2)};
    auto apply = mover.get_apply();
    MultiBlockCube<2> cube;
    assert(cube.is_solved());
    apply(U, cube);
    assert(!cube.is_solved());
    Algorithm seq({R, U, D, B2});
    mover.apply(seq, cube);
    mover.apply(U, cube);
}

int main() {
    mover_first_test();
    return 0;
}