#include "2gen.hpp"

int main() {
    CubieCube cube;
    cube.apply("R' U R' U' R' U' R' U R U R2");
    for (const Pairing& key : pairings) {
        Pairing value = get_pairing(cube, key);
        assert(value == key);
    }

    cube.apply("R U R' U2 R' U R2 U R U' R2");
    print_array(get_pairing_permutation(cube));

    // cube.apply("F R F'");
    // print_array(get_pairing_permutation(cube));
    return 0;
}