#include "2gen.hpp"

int main() {
    CubieCube cube;
    cube.apply("R' U R' U' R' U' R' U R U R2");
    for (const Pairing& key : pairings) {
        Pairing value = get_pairing(cube, key);
        assert(value == key);
    }

    cube = CubieCube();
    for (unsigned k = 0; k < 105; ++k) {
        cube.apply("R U");
    }

    assert(two_gen_index(cube) == 0);

    make_two_gen_pruning_table();
    return 0;
}