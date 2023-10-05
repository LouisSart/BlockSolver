#include "search.hpp"

#include "algorithm.hpp"
#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "pruning_table.hpp"

void test_object_sizes() {
    std::cout << "size of CoordinateBlockCube: " << sizeof(CoordinateBlockCube)
              << std::endl;
    std::cout << "size of 2 ints: " << 2 * sizeof(int) << std::endl;
    std::cout << "size of std::vector<Move>: " << sizeof(std::vector<Move>)
              << std::endl;
    std::cout << "size of Node<CoordinateBlockCube>: "
              << sizeof(Node<CoordinateBlockCube>) << std::endl;

    // Note: next assertion can fail if the compiler optimizes padding of the
    // memory layout
    assert(sizeof(Node<CoordinateBlockCube>) ==
           sizeof(std::vector<Move>) + 2 * sizeof(int) +
               sizeof(CoordinateBlockCube));
}

void test_expand_cbc() {
    CoordinateBlockCube cbc;
    Block<4, 2> b("FrontColumns", {0, 1, 4, 5}, {4, 5});
    BlockMoveTable table(b);

    Node root(cbc, 0);
    auto children = root.expand(
        [&table](const Move& move, CoordinateBlockCube& CBC) {
            table.apply(move, CBC);
        },
        HTM_Moves);

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
        [&table](const Move& move, CoordinateBlockCube& CBC) {
            table.apply(move, CBC);
        },
        [&k](const CoordinateBlockCube& CBC) {
            ++k;
            return 18 - k;
        },
        HTM_Moves);

    assert(std::is_sorted(
        children.begin(), children.end(),
        [](Node<CoordinateBlockCube> node1, Node<CoordinateBlockCube> node2) {
            return node1.estimate > node2.estimate;
        }));
}

void test_breadth_first_search() {
    Block<2, 3> b("RouxFirstBlock", {4, 7}, {4, 7, 11});
    CoordinateBlockCube cbc;
    BlockMoveTable table(b);
    table.apply(Algorithm({F2, D, R, U}), cbc);

    Node<CoordinateBlockCube> root(cbc, 0);
    auto solutions = breadth_first_search(
        root,
        [&table](const Move& move, CoordinateBlockCube& CBC) {
            table.apply(move, CBC);
        },
        4);
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
    auto solutions = depth_first_search(root, table, NullPruningTable(), 3);
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
    auto solutions = depth_first_search(root, m_table, p_table, 3);
    for (auto&& s : solutions) {
        s.show();
    }
}

void test_solve_222_on_wr_scramble() {
    Block<1, 3> b("DLB_222", {7}, {7, 10, 11});
    CoordinateBlockCube cbc;
    BlockMoveTable m_table(b);
    OptimalPruningTable p_table(b);
    Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                        U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
    scramble.show();
    m_table.apply(scramble, cbc);

    Node<CoordinateBlockCube> root(cbc, 0);
    auto solutions = depth_first_search(root, m_table, p_table, 4);
    assert(solutions.size() == 1);
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
    std::cout << " --- Test depth first 2x2 solve on Wen's WR scramble #1 --- "
              << std::endl;
    test_solve_222_on_wr_scramble();
    std::cout << " --- Test object sizes --- " << std::endl;
    test_object_sizes();
    return 0;
}