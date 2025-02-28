#include "2gen.hpp"

int main() {
    assert(pairing_index(CubieCube()) == 0);

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

    make_corner_index_table();
    for (unsigned k : two_gen_corner_index_table) {
        assert(k < 40321);
    }
    std::set<unsigned> check_duplicates{two_gen_corner_index_table.begin(),
                                        two_gen_corner_index_table.end()};
    assert(check_duplicates.size() == 120);
    return 0;
}