// #include "search.cpp"
// #include"pruning_table.hpp"
#include "coordinate_block_cube.hpp"

int main()
{
  constexpr uint nc=1;
  constexpr uint ne=3;
  const Block<nc,ne> b("DLB_222",{7},{7,10,11});
  BlockCube<nc,ne> bc(b);
  BlockMoveTable<nc, ne> mt;
  mt.init(b);
  // PruningTable<nc,ne> pt(b);
  CoordinateBlockCube cbc(mt);
  return 0;
}
