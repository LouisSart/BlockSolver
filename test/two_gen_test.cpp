#include "two_gen.hpp"

#include "search.hpp"

namespace tg = two_gen;

void pairing_test() {
    assert(tg::pairing_index(CubieCube()) == 0);

    CubieCube cube;
    cube.apply("R' U R' U' R' U' R' U R U R2");
    for (const tg::Pairing& key : tg::pairings) {
        tg::Pairing value = tg::get_pairing(cube, key);
        assert(value == key);
    }
}  // namespace )

void two_gen_index_test() {
    CubieCube cube;
    for (unsigned k = 0; k < 105; ++k) {
        cube.apply("R U");
    }

    assert(tg::index(cube) == 0);
}

void corner_index_test() {
    for (unsigned k : tg::corner_index_table) {
        assert(k < 40321);
    }
    std::set<unsigned> check_duplicates{tg::corner_index_table.begin(),
                                        tg::corner_index_table.end()};
    assert(check_duplicates.size() == 120);
}

void pruning_table_test() {
    tg::make_pruning_table(tg::corner_ptable, tg::corner_index, tg::moves);
    tg::make_pruning_table(tg::edge_ptable, tg::edge_index, tg::moves);
}

void two_gen_solve_test() {
    auto root = two_gen::initialize("R U R U R U R U R U R U R U R");

    auto solutions = two_gen::solve(root, 20, 0);

    solutions.sort_by_depth();
    solutions.show();
    solutions.show();
}

int main() {
    pairing_test();
    two_gen_index_test();
    tg::make_corner_index_table();
    pruning_table_test();
    corner_index_test();
    two_gen_solve_test();
    return 0;
}