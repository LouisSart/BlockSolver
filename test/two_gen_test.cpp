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

void two_gen_finish_test() {
    two_gen::load_tables();
    auto root = two_gen::initialize("B L B L B L B L B L B L B L B");

    auto solutions = two_gen::solve(root, 20, 0);
    assert(solutions.size() == 2);
    assert(solutions[0]->depth == 15);
}

void two_gen_reduction_index_test() {
    namespace b223 = block_solver_223;
    two_gen_reduction::load_tables();
    auto root = two_gen_reduction::cc_initialize(CubieCube());
    auto cube = root->state;

    assert(two_gen_reduction::is_solved(cube));
    for (auto move : {R, U, R3, U2, R}) {
        two_gen_reduction::apply(move, cube);
    }
    assert(two_gen_reduction::is_solved(cube));
    assert(two_gen_reduction::estimate(cube) == 0);

    for (auto move : {F, B3, R2, F3, B}) {
        two_gen_reduction::apply(move, cube);
    }
    assert(two_gen_reduction::is_solved(cube));
    assert(two_gen_reduction::estimate(cube) == 0);

    two_gen_reduction::apply(F, cube);
    assert(two_gen_reduction::max_estimate(cube[1]) == 1);
    two_gen_reduction::apply(B, cube);
    assert(two_gen_reduction::max_estimate(cube[1]) == 2);
}

void two_gen_reduction_solve_test() {
    auto root = two_gen_reduction::initialize("R U R U R' U2 R' B D R U");

    auto solutions = two_gen_reduction::solve(root, 20, 0);
    assert(solutions.size() == 1);
    assert(solutions[0]->depth == 4);
}

void two_gen_solve_test() {
    auto solutions = two_gen_solve(
        "R' U' F U' R' D L' U2 B' R2 U2 D' B2 R2 U' R2 U L2 B2 L F' R' U' F",
        25, 0);
    assert(solutions.size() == 4);
    assert(solutions[0][0].size() == 10);
    assert(solutions[0][1].size() == 15);
}

int main() {
    pairing_test();
    two_gen_index_test();
    corner_index_test();
    two_gen_finish_test();
    two_gen_reduction_index_test();
    two_gen_reduction_solve_test();
    two_gen_solve_test();
    return 0;
}