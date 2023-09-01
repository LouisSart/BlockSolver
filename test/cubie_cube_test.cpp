#include "cubie_cube.hpp"

#include <cassert>

#include "algorithm.hpp"

int main() {
    auto cube = CubieCube();
    auto no_effect = Algorithm({R, U, R3, F3, U2, L3, U3, L, F, U2});
    auto Tperm = Algorithm({R, U, R3, U3, R3, F, R2, U3, R3, U3, R, U, R3, F3});
    cube.apply(no_effect);
    assert(CubieCube() == cube);
    cube.apply(Tperm);
    cube.apply(Tperm);

    assert(CubieCube() == cube);
    cube.show();
    return 0;
}
