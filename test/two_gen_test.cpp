#include "two_gen.hpp"

#include "search.hpp"

void pairing_test() {
    assert(two_gen::pairing_index(CubieCube()) == 0);

    CubieCube cube;
    cube.apply("R' U R' U' R' U' R' U R U R2");
    for (const two_gen::Pairing& key : two_gen::pairings) {
        two_gen::Pairing value = two_gen::get_pairing(cube, key);
        assert(value == key);
    }
}  // namespace )

void two_gen_index_test() {
    CubieCube cube;
    for (unsigned k = 0; k < 105; ++k) {
        cube.apply("R U");
    }

    assert(two_gen::index(cube) == 0);
}

void corner_index_test() {
    two_gen::make_corner_index_table();
    for (unsigned k : two_gen::corner_index_table) {
        assert(k < 40320);
    }
    std::set<unsigned> check_duplicates{two_gen::corner_index_table.begin(),
                                        two_gen::corner_index_table.end()};
    assert(check_duplicates.size() == 120);
}

void pruning_table_test() {
    two_gen::make_pruning_table(two_gen::corner_ptable, two_gen::corner_index,
                                two_gen::moves);
    two_gen::make_pruning_table(two_gen::edge_ptable, two_gen::edge_index,
                                two_gen::moves);
}

void two_gen_solve_test() {
    auto root = two_gen::initialize("R U R U R U R U R U R U R U R");

    auto solutions = two_gen::solve(root, 20, 0);

    solutions.sort_by_depth();
    solutions.show();
}

void two_gen_reduction_index_test() {
    auto root = two_gen_reduction::cc_initialize(CubieCube());
    auto cube = root->state;

    assert(two_gen_reduction::is_solved(cube));
    for (auto move : {F, B3, R2, F3, B, U2}) {
        two_gen_reduction::apply(move, cube);
    }
    assert(two_gen_reduction::is_solved(cube));
    two_gen_reduction::apply(R, cube);
    assert(!two_gen_reduction::is_solved(cube));
}

void two_gen_reduction_table_test() {
    two_gen_reduction::make_corner_equivalence_table();
    two_gen_reduction::make_pruning_table();
    two_gen_reduction::write_tables();
    two_gen_reduction::load_tables();
}

int main() {
    pairing_test();
    two_gen_index_test();
    corner_index_test();
    pruning_table_test();
    two_gen_solve_test();
    two_gen_reduction_index_test();
    two_gen_reduction_table_test();
    return 0;
}