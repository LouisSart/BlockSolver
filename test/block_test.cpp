#include "algorithm.hpp"
#include "block_cube.hpp"
#include "cubie_cube.hpp"

int main() {
    const Block<1, 3> b("DLB_222", {7}, {7, 10, 11});
    BlockCube<1, 3> bc(b);
    CubieCube cc;
    Algorithm alg({D2, L, R, F3, U});
    alg.apply(cc);
    bc.from_cubie_cube(cc);
    bc.show();
    return 0;
}
