#include "2gen.hpp"

#include "search.hpp"

void pairing_test() {
    assert(pairing_index(CubieCube()) == 0);

    CubieCube cube;
    cube.apply("R' U R' U' R' U' R' U R U R2");
    for (const Pairing& key : pairings) {
        Pairing value = get_pairing(cube, key);
        assert(value == key);
    }
}

void two_gen_index_test() {
    CubieCube cube;
    for (unsigned k = 0; k < 105; ++k) {
        cube.apply("R U");
    }

    assert(two_gen_index(cube) == 0);
}
void corner_index_test() {
    for (unsigned k : two_gen_corner_index_table) {
        assert(k < 40321);
    }
    std::set<unsigned> check_duplicates{two_gen_corner_index_table.begin(),
                                        two_gen_corner_index_table.end()};
    assert(check_duplicates.size() == 120);
}

void pruning_table_test() {
    make_pruning_table(two_gen_corner_ptable, two_gen_corner_index,
                       two_gen_moves);
    make_pruning_table(two_gen_edge_ptable, two_gen_edge_index, two_gen_moves);
}

bool is_solved(const CubieCube& cc) { return cc.is_solved(); }
void apply(const Move& move, CubieCube& cc) { cc.apply(move); }

std::vector<Move> two_gen_directions(const Node<CubieCube>::sptr node) {
    if (node->parent == nullptr) {
        return {R, R2, R3, U, U2, U3};
    } else if (node->last_moves.back() == R || node->last_moves.back() == R2 ||
               node->last_moves.back() == R3) {
        return {U, U2, U3};
    } else {
        return {R, R2, R3};
    }
}

void two_gen_solve_test() {
    CubieCube cc;
    cc.apply("R U R U R U R U R U R U R U R");

    auto root = make_root(cc);
    auto solutions =
        IDAstar(root, apply, two_gen_estimate, is_solved, two_gen_directions);
    solutions.show();
}

int main() {
    pairing_test();
    two_gen_index_test();
    make_corner_index_table();
    pruning_table_test();
    corner_index_test();
    two_gen_solve_test();
    return 0;
}