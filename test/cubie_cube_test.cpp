#include "cubie_cube.hpp"

#include <cassert>

#include "algorithm.hpp"

auto no_effect = Algorithm({R, U, R3, F3, U2, L3, U3, L, F, U2});
auto Tperm = Algorithm({R, U, R3, U3, R3, F, R2, U3, R3, U3, R, U, R3, F3});
auto corner_3_cycle = Algorithm({R, U, R3, D, R, U3, R3, D3});
auto edge_3_cycle = Algorithm({R, L3, U2, R3, L, F2});
auto random_moves = Algorithm({R, F3, U, D3, U, R, B3, U3, R3, D3, F2, L2});

void test_move_apply() {
    auto cube = CubieCube();

    cube.apply(no_effect);
    assert(CubieCube() == cube);
    cube.apply(Tperm);
    cube.apply(Tperm);

    assert(CubieCube() == cube);
}

void test_parity() {
    auto cube = CubieCube();

    cube.apply(corner_3_cycle);
    assert(cube.corner_parity() == 1);

    cube.apply(edge_3_cycle);
    assert(cube.edge_parity() == 1);

    cube.apply(Tperm);
    assert(cube.corner_parity() == -1);
    assert(cube.edge_parity() == -1);

    cube.apply(no_effect);
    assert(cube.corner_parity() == -1);
    assert(cube.edge_parity() == -1);

    cube.apply(random_moves);
    assert(cube.edge_parity() == -1);
    assert(cube.corner_parity() == -1);

    assert(cube.is_solvable());
}

void test_random_state() {
    auto cube = CubieCube::random_state();
    assert(cube.is_solvable());
    assert(!cube.is_solved());  // This could be true once in while tho...
}

void test_inverse() {
    auto solved = CubieCube();
    auto cube = CubieCube::random_state();
    auto inverse = cube.inverse();
    cube.apply(inverse);
    assert(cube.is_solved());
}

void test_symmetries() {
    CubieCube random = CubieCube::random_state();
    for (int s = 18; s < 25; ++s) {
        CubieCube cube;
        auto sym = elementary_transformations[s];
        cube.apply(sym);
        cube.apply(random);
        cube.apply(sym.inverse());
        cube.apply(sym);
        cube.apply(random.inverse());
        cube.apply(sym.inverse());
        assert(cube.is_solved());
    }
}

int main() {
    test_move_apply();
    test_parity();
    test_random_state();
    test_inverse();
    test_symmetries();
    return 0;
}
