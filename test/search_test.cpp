#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "algorithm.hpp"

int main() {
    CoordinateBlockCube cbc;
    Block<4, 2> b("FrontColumns", {0, 1, 4, 5}, {4, 5});
    BlockMoveTable table(b);

    Node root(cbc, 0);
    root.show();
    auto children = root.expand(
        [table](Move move, CoordinateBlockCube cbc){
            return table.apply(move,cbc);}, 
        {R, U, F}
    );
    return 0;
}