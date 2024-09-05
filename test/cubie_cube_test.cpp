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

    cube = CubieCube(no_effect);
    assert(cube.is_solved());

    cube = CubieCube(edge_3_cycle);
    cube.apply(edge_3_cycle);
    cube.apply(edge_3_cycle);
    assert(cube.is_solved());

    CubieCube cube_check;
    cube.apply(Algorithm{U, R, F, L, B, D});
    cube_check.apply(U);
    cube_check.apply(R);
    cube_check.apply(F);
    cube_check.apply(L);
    cube_check.apply(B);
    cube_check.apply(D);

    assert(cube == cube_check);
}

void test_copy_constructor() {
    CubieCube cc = CubieCube::random_state();
    CubieCube cc_copy;

    cc_copy = cc;
    assert(&cc != &cc_copy);
    assert(cc == cc_copy);

    cc_copy.apply(random_moves);
    assert(cc != cc_copy);

    cc.apply(random_moves);
    assert(cc == cc_copy);
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
    assert(!cube.is_solved());  // This could fail once in a while tho...
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

void test_conjugation() {
    auto cube = CubieCube::random_state();

    for (unsigned s = 0; s < N_SYM; ++s) {
        assert(cube == cube.get_conjugate(s).get_anti_conjugate(s));
    }
}

int main() {
    test_move_apply();
    test_copy_constructor();
    test_parity();
    test_random_state();
    test_inverse();
    test_conjugation();
    return 0;
}
