#include "search.hpp"

#include "algorithm.hpp"
#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "pruning_table.hpp"  // shared_ptr
#include "utils.hpp"          // check_solutions

template <typename Block, typename NodePtr>
void check_solutions(const Solutions<NodePtr>& solutions,
                     const Algorithm& scramble, Block& b) {
    assert(solutions.size() > 0);
    CubieCube scrambled(scramble), check;
    for (auto&& s : solutions) {
        check = scrambled;
        check.apply(s->get_path());
        assert(b.is_solved(b.to_coordinate_block_cube(check)));
    }
}

void test_expand_cbc() {
    auto b = Block<4, 2>("FrontColumns", {ULF, DLF, URF, DRF}, {LF, RF});

    CoordinateBlockCube cbc = b.get_solved_cbc();
    BlockMoveTable m_table(b);

    auto root = make_root(cbc);  // shared_ptr
    auto children = root->expand(m_table.get_apply(), HTM_Moves);

    assert(children.size() == 18);
    assert(!b.is_solved(children[U]->state));
    assert(!b.is_solved(children[D]->state));
    assert(!b.is_solved(children[R]->state));
    assert(!b.is_solved(children[L]->state));
    assert(!b.is_solved(children[F]->state));
    assert(b.is_solved(children[B]->state));
    assert(b.is_solved(children[B2]->state));
    assert(b.is_solved(children[B3]->state));
}

void test_successive_expansions() {
    Block<1, 3> b("DLB_222", {DLB}, {LB, DB, DL});
    CoordinateBlockCube cbc = b.get_solved_cbc();
    BlockMoveTable m_table(b);
    PruningTable p_table = load_pruning_table(b);
    Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                        U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
    m_table.apply(scramble, cbc);

    auto root = make_root(cbc);
    auto mover = m_table.get_apply();
    auto pruner = p_table.get_estimator();
    auto children = root->expand(mover, HTM_Moves);
    assert(pruner(root->state) == 4);
    assert(pruner(children[U]->state) == 5);
    assert(pruner(children[D3]->state) == 3);
    children = children[D3]->expand(mover, HTM_Moves);
    assert(pruner(children[L2]->state) == 2);
    children = children[L2]->expand(mover, HTM_Moves);
    assert(pruner(children[B]->state) == 1);
    children = children[B]->expand(mover, HTM_Moves);
    assert(pruner(children[L3]->state) == 0);
}

void test_depth_first_search() {
    Algorithm scramble({R, L2, D, F});
    Block<1, 3> b("DLB_222", {DLB}, {LB, DB, DL});
    CoordinateBlockCube cbc = b.get_scrambled_cbc(scramble);
    BlockMoveTable m_table(b);

    auto root = Solutions<Node<CoordinateBlockCube>::sptr>({make_root(cbc)});
    auto solutions = depth_first_search(
        root, m_table.get_apply(),
        [](const CoordinateBlockCube& cube) { return (unsigned)1; },
        b.get_is_solved(), 3);
    solutions.show();
    assert(solutions.size() == 2);
    check_solutions(solutions, scramble, b);
}

void test_depth_first_search_with_heuristic() {
    Algorithm scramble({R, L2, D, F});
    Block<1, 3> b("DLB_222", {DLB}, {LB, DB, DL});
    CoordinateBlockCube cbc = b.get_scrambled_cbc(scramble);
    BlockMoveTable m_table(b);
    PruningTable p_table = load_pruning_table(b);

    auto root = Solutions<Node<CoordinateBlockCube>::sptr>({make_root(cbc)});
    auto solutions =
        depth_first_search(root, m_table.get_apply(), p_table.get_estimator(),
                           b.get_is_solved(), 3);
    solutions.show();

    check_solutions(solutions, scramble, b);
}

void test_solve_222_on_wr_scramble() {
    Block<1, 3> b("DLB_222", {DLB}, {LB, DB, DL});
    BlockMoveTable m_table(b);
    PruningTable p_table = load_pruning_table(b);

    Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                        U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
    CoordinateBlockCube cbc = b.get_scrambled_cbc(scramble);
    scramble.show();

    auto root = Solutions<Node<CoordinateBlockCube>::sptr>({make_root(cbc)});
    auto solutions =
        depth_first_search(root, m_table.get_apply(), p_table.get_estimator(),
                           b.get_is_solved(), 4);
    solutions.show();
    assert(solutions.size() == 1);
    check_solutions(solutions, scramble, b);
}

void test_symmetry_solve() {
    Block<1, 3> b("DLB_222", {DLB}, {LB, DB, DL});
    BlockMoveTable m_table(b);
    PruningTable p_table = load_pruning_table(b);

    Algorithm scramble({R3, U3, F,  D2, R2, F3, L2, D2, F3, L,  U3, B,
                        U3, D3, F2, B2, L2, D,  F2, U2, D,  R3, U3, F});
    scramble.show();
    unsigned sym_index = symmetry_index(0, 2, 1, 0);
    CoordinateBlockCube cbc = b.to_coordinate_block_cube(
        CubieCube(scramble).get_conjugate(sym_index));

    auto root = Solutions<Node<CoordinateBlockCube>::sptr>({make_root(cbc)});
    auto solutions =
        depth_first_search(root, m_table.get_sym_apply(sym_index),
                           p_table.get_estimator(), b.get_is_solved(), 5);
    solutions.show();
    assert(solutions.size() == 2);
}

int main() {
    std::cout << " --- Test expansion ---" << std::endl;
    test_expand_cbc();
    std::cout << " --- Test successive expansions ---" << std::endl;
    test_successive_expansions();
    std::cout << " --- Test depth first --- " << std::endl;
    test_depth_first_search();
    std::cout << " --- Test depth first with heuristic --- " << std::endl;
    test_depth_first_search_with_heuristic();
    std::cout << "--- Test depth first 2x2 solve on Wen's WR scramble #1 ---"
              << std::endl;
    test_solve_222_on_wr_scramble();
    std::cout << "--- Test solve with symmetry conjugation ---" << std::endl;
    test_symmetry_solve();
    return 0;
}