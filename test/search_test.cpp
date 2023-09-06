#include <deque>
#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "algorithm.hpp"

void test_expand_cbc() {
    CoordinateBlockCube cbc;
    Block<4, 2> b("FrontColumns", {0, 1, 4, 5}, {4, 5});
    BlockMoveTable table(b);

    Node root(cbc, 0);
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

void test_breadth_first_search() {
    Block<2, 3> b("RouxFirstBlock", {4, 7}, {4, 7, 11});
    CoordinateBlockCube cbc;
    BlockMoveTable table(b);
    table.apply(Algorithm({L, U}), cbc);

    Node<CoordinateBlockCube> root(cbc, 0);
    std::deque<Node<CoordinateBlockCube>> queue = {root};

    while (queue.size() != 0) {
        auto node = queue.back();
        if (node.cube.is_solved()) { 
            std::cout << "Solution found :" << std::endl;
            node.show();
            return;
        }
        else {
            queue.pop_back();
            auto children = node.expand(
                [table](const Move& move, CoordinateBlockCube& CBC){table.apply(move,CBC);},
                HTM_Moves
            );
            for (auto&& child : children) {
                queue.push_front(child);
            }
        }
        assert(queue.size() < 10000); // Avoiding infinite loop
    }
}

int main() {
    test_expand_cbc();
    test_breadth_first_search();
    return 0;
}