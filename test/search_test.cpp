#include "search.hpp"

#include "algorithm.hpp"
#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "pruning_table.hpp"  // shared_ptr

template <typename MoveTable, typename SolutionContainer>
void check_solutions(const Algorithm& scramble, const MoveTable& m_table,
                     const SolutionContainer& solutions) {
    for (auto&& s : solutions) {
        CoordinateBlockCube cbc_check;
        m_table.apply(scramble, cbc_check);
        m_table.apply(s, cbc_check);
        assert(cbc_check.is_solved());
    }
}

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

template <typename Block>
void test_expand_cbc(const Block& b) {
    CoordinateBlockCube cbc;
    BlockMoveTable table(b);

    auto root = Node<CoordinateBlockCube>::make_root(cbc);  // shared_ptr
    auto children = root->expand(
        [&table](const Move& move, CoordinateBlockCube& CBC) {
            table.apply(move, CBC);
        },
        HTM_Moves);

    assert(children.size() == 18);
    assert(!children[U]->state.is_solved());
    assert(!children[D]->state.is_solved());
    assert(!children[R]->state.is_solved());
    assert(!children[L]->state.is_solved());
    assert(!children[F]->state.is_solved());
    assert(children[B]->state.is_solved());
    assert(children[B2]->state.is_solved());
    assert(children[B3]->state.is_solved());
}

template <typename Block>
void test_expand_cbc_with_heuristic(const Block& b) {
    CoordinateBlockCube cbc;
    BlockMoveTable table(b);
    int k = 0;

    auto root = Node<CoordinateBlockCube>::make_root(cbc);
    auto children = root->expand(
        [&table](const Move& move, CoordinateBlockCube& CBC) {
            table.apply(move, CBC);
        },
        [&k](const CoordinateBlockCube& CBC) {
            ++k;
            return 18 - k;
        },
        HTM_Moves);

    assert(std::is_sorted(children.begin(), children.end(),
                          [](Node<CoordinateBlockCube>::sptr node1,
                             Node<CoordinateBlockCube>::sptr node2) {
                              return node1->estimate > node2->estimate;
                          }));
}

// void test_breadth_first_search() {
//     Algorithm scramble({F2, D, R, U});
//     Block<2, 3> b("RouxFirstBlock", {DLF, DLB}, {LF, LB, DL});
//     CoordinateBlockCube cbc;
//     BlockMoveTable m_table(b);
//     m_table.apply(scramble, cbc);

//     Node<CoordinateBlockCube> root(cbc, 0);
//     auto solutions = breadth_first_search(
//         root,
//         [&m_table](const Move& move, CoordinateBlockCube& CBC) {
//             m_table.apply(move, CBC);
//         },
//         [](const CoordinateBlockCube& cube) { return cube.is_solved(); }, 4);
//     show(solutions);
//     check_solutions(scramble, m_table, solutions);
// }

// void test_depth_first_search() {
//     Algorithm scramble({R, L2, D, F});
//     Block<1, 3> b("DLB_222", {DLB}, {LB, DB, DL});
//     CoordinateBlockCube cbc;
//     BlockMoveTable m_table(b);
//     m_table.apply(scramble, cbc);

//     Node<CoordinateBlockCube> root(cbc, 0);
//     auto solutions = depth_first_search(
//         root,
//         [&m_table](const Move& move, CoordinateBlockCube& cube) {
//             m_table.apply(move, cube);
//         },
//         [](const CoordinateBlockCube& cube) { return (unsigned)1; },
//         [](const CoordinateBlockCube& cube) { return cube.is_solved(); }, 3);
//     show(solutions);
//     check_solutions(scramble, m_table, solutions);
// }

// void test_depth_first_search_with_heuristic() {
//     Algorithm scramble({R, L2, D, F});
//     Block<1, 3> b("DLB_222", {DLB}, {LB, DB, DL});
//     CoordinateBlockCube cbc;
//     BlockMoveTable m_table(b);
//     PruningTable p_table = Strategy::Optimal(b).load_table();
//     m_table.apply(scramble, cbc);

//     Node<CoordinateBlockCube> root(cbc, 0);
//     auto solutions = depth_first_search(
//         root,
//         [&m_table](const Move& move, CoordinateBlockCube& cube) {
//             m_table.apply(move, cube);
//         },
//         [&p_table](const CoordinateBlockCube& cube) {
//             return p_table.get_estimate(cube);
//         },
//         [](const CoordinateBlockCube& cube) { return cube.is_solved(); }, 3);
//     show(solutions);
//     check_solutions(scramble, m_table, solutions);
// }

// void test_solve_222_on_wr_scramble() {
//     Block<1, 3> b("DLB_222", {DLB}, {LB, DB, DL});
//     CoordinateBlockCube cbc;
//     BlockMoveTable m_table(b);
//     PruningTable p_table = Strategy::Optimal(b).load_table();
//     Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
//                         U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
//     scramble.show();
//     m_table.apply(scramble, cbc);

//     Node<CoordinateBlockCube> root(cbc, 0);
//     auto solutions = depth_first_search(
//         root,
//         [&m_table](const Move& move, CoordinateBlockCube& cube) {
//             m_table.apply(move, cube);
//         },
//         [&p_table](const CoordinateBlockCube& cube) {
//             return p_table.get_estimate(cube);
//         },
//         [](const CoordinateBlockCube& cube) { return cube.is_solved(); }, 4);
//     assert(solutions.size() == 1);
//     show(solutions);
//     check_solutions(scramble, m_table, solutions);
// }

// void test_solve_with_split_heuristic() {
//     Block<1, 3> b("DLB_222", {DLB}, {LB, DB, DL});
//     CoordinateBlockCube cbc;
//     BlockMoveTable m_table(b);
//     Strategy::Split strat(b);
//     auto p_table = strat.load_table();
//     Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
//                         U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
//     scramble.show();
//     m_table.apply(scramble, cbc);
//     auto root = Node(cbc, 0);
//     auto solutions = IDAstar(
//         root,
//         [&m_table](const Move& move, CoordinateBlockCube& cube) {
//             m_table.apply(move, cube);
//         },
//         [&p_table](const CoordinateBlockCube& cube) {
//             return p_table.get_estimate(cube);
//         },
//         [](const CoordinateBlockCube& cube) { return cube.is_solved(); }, 4);
//     assert(solutions.size() == 1);
//     show(solutions);
//     check_solutions(scramble, m_table, solutions);
// }

// struct NodePlus : public std::enable_shared_from_this<NodePlus> {
//     using ptr = std::shared_ptr<NodePlus>;
//     unsigned depth;
//     ptr parent;

//     NodePlus(unsigned d) : depth{d} {}
//     NodePlus(unsigned d, const ptr p) : depth{d}, parent{p} {}

//     ptr make_child() {
//         auto child = std::make_shared<NodePlus>(depth + 1,
//         shared_from_this()); return child;
//     }
// };

// auto make_root(unsigned d) {
//     return std::shared_ptr<NodePlus>(new NodePlus(d));
// }

// NodePlus::ptr make_family() {
//     NodePlus::ptr root = make_root(0);
//     NodePlus::ptr child = root;
//     for (auto k = 0; k < 10; ++k) {
//         child = child->make_child();
//     }
//     return child;
// }

int main() {
    Block<2, 3> RouxFirstBlock("RouxFirstBlock", {DLF, DLB}, {LF, LB, DL});
    Block<4, 2> FrontColumns("FrontColumns", {ULF, DLF, URF, DRF}, {LF, RF});

    std::cout << " --- Test expansion ---" << std::endl;
    test_expand_cbc(FrontColumns);
    std::cout << " --- Test expansion with heuristic ---" << std::endl;
    test_expand_cbc_with_heuristic(RouxFirstBlock);

    // std::cout << " --- Test breadth first ---" << std::endl;
    // test_breadth_first_search();
    // std::cout << " --- Test depth first --- " << std::endl;
    // test_depth_first_search();
    // std::cout << " --- Test depth first with heuristic --- " << std::endl;
    // test_depth_first_search_with_heuristic();
    // std::cout << "--- Test depth first 2x2 solve on Wen's WR scramble #1 ---
    // "
    //           << std::endl;
    // test_solve_222_on_wr_scramble();
    // std::cout << " --- Test object sizes --- " << std::endl;
    // test_object_sizes();
    // std::cout << " --- Test split heuristic --- " << std::endl;
    // test_solve_with_split_heuristic();
    return 0;
}