#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "algorithm.hpp"

void test_expand_cbc() {
    CoordinateBlockCube cbc;
    Block<4, 2> b("FrontColumns", {0, 1, 4, 5}, {4, 5});
    BlockMoveTable table(b);

    Node root(cbc, 0);
    root.show();
    auto children = root.expand(
        [table](const Move& move, CoordinateBlockCube& CBC){table.apply(move,CBC);},
        HTM_Moves
    );
    
    assert(children.size() == 18);
    assert(!children[U].cube.is_solved());
    assert(!children[D].cube.is_solved());
    assert(!children[R].cube.is_solved());
    assert(!children[L].cube.is_solved());
    assert(!children[F].cube.is_solved());
    assert(children[B].cube.is_solved());
    assert(children[B2].cube.is_solved());
    assert(children[B3].cube.is_solved());
}

// void test_breadth_first_search() {
//     Block<2, 3> b("RouxFirstBlock", {4, 7}, {4, 7, 11});
//     CoordinateBlockCube cbc;
//     BlockMoveTable table(b);

// }

int main() {
    test_expand_cbc();
    return 0;
}