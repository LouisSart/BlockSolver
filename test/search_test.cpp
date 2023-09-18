#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "search.hpp"
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
    assert(!children[U].state.is_solved());
    assert(!children[D].state.is_solved());
    assert(!children[R].state.is_solved());
    assert(!children[L].state.is_solved());
    assert(!children[F].state.is_solved());
    assert(children[B].state.is_solved());
    assert(children[B2].state.is_solved());
    assert(children[B3].state.is_solved());
}

void test_expand_cbc_with_heuristic() {
    CoordinateBlockCube cbc;
    Block<4, 2> b("FrontColumns", {0, 1, 4, 5}, {4, 5});
    BlockMoveTable table(b);
    int k = 0;

    Node root(cbc, 0);
    auto children = root.expand(
        [table](const Move& move, CoordinateBlockCube& CBC){table.apply(move,CBC);},
        [&k](const CoordinateBlockCube& CBC){++k; return 18 - k; },
        HTM_Moves
    );

    assert(std::is_sorted(
      children.begin(),
      children.end(),
      [](Node<CoordinateBlockCube> node1, Node<CoordinateBlockCube> node2){
        return node1.estimate > node2.estimate;
      }
    ));
}

void test_breadth_first_search() {
    Block<2, 3> b("RouxFirstBlock", {4, 7}, {4, 7, 11});
    CoordinateBlockCube cbc;
    BlockMoveTable table(b);
    table.apply(Algorithm({F2, D, R, U}), cbc);

    Node<CoordinateBlockCube> root(cbc, 0);
    auto solutions = breadth_first_search(
        root,
        [table](const Move& move, CoordinateBlockCube& CBC){table.apply(move,CBC);},
        4
    );
    for (auto&& s : solutions) {
        s.show();
    }
}

void test_depth_first_search() {
    Block<1, 3> b("DLB_222", {7}, {7, 10, 11});
    CoordinateBlockCube cbc;
    BlockMoveTable table(b);
    table.apply(Algorithm({R, L2, D, F}), cbc);

    Node<CoordinateBlockCube> root(cbc, 0);
    auto solutions = depth_first_search(
        root,
        table,
        NullPruningTable(),
        3
    );
    for (auto&& s : solutions) {
        s.show();
    }
}

void test_depth_first_search_with_heuristic() {
    Block<1, 3> b("DLB_222", {7}, {7, 10, 11});
    CoordinateBlockCube cbc;
    BlockMoveTable m_table(b);
    OptimalPruningTable p_table(b);
    m_table.apply(Algorithm({R, L2, D, F}), cbc);

    Node<CoordinateBlockCube> root(cbc, 0);
    auto solutions = depth_first_search(
        root,
        m_table,
        p_table,
        3
    );
    for (auto&& s : solutions) {
        s.show();
    }
}

int main() {
    std::cout << " --- Test expansion ---" << std::endl;
    test_expand_cbc();
    std::cout << " --- Test expansion with heuristic ---" << std::endl;
    test_expand_cbc_with_heuristic();
    std::cout << " --- Test breadth first ---" << std::endl;
    test_breadth_first_search();
    std::cout << " --- Test depth first --- " << std::endl;
    test_depth_first_search();
    std::cout << " --- Test depth first with heuristic --- " << std::endl;
    test_depth_first_search_with_heuristic();
    return 0;
}