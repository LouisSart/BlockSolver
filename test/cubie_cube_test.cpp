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
    assert(cube.is_solved());
    cube.apply(Tperm);
    cube.apply(Tperm);

    assert(cube.is_solved());
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
    assert(!cube.is_solved());  // This could be true once in a while tho...
}

void test_inverse() {
    auto cube = CubieCube::random_state();
    cube.apply(cube.get_inverse());
    assert(cube.is_solved());

    auto inverse = cube;
    inverse.inverse();
    cube.apply(inverse);
    assert(cube.is_solved());
}

void test_symmetries() {
    CubieCube random = CubieCube::random_state();
    CubieCube cube;
    for (unsigned s = x; s <= z3; ++s) {
        auto sym = move_cc[s];
        cube.apply(sym.get_inverse());
        cube.apply(random);
        cube.apply(sym);
        cube.inverse();
        cube.apply(sym.get_inverse());
        cube.apply(random);
        cube.apply(sym);
        assert(cube.is_solved());
    };

    // RL-mirror cannot be applied as a legal cube permutation
    cube.RL_mirror();
    cube.apply(random);
    cube.RL_mirror();
    cube.inverse();
    cube.RL_mirror();
    cube.apply(random);
    cube.RL_mirror();
    assert(cube.is_solved());
}

int main() {
    test_move_apply();
    test_parity();
    test_random_state();
    test_inverse();
    test_symmetries();
    return 0;
}
