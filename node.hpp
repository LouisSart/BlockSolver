#include "coordinate_block_cube.hpp"

class Node
{
private:
  CoordinateBlockCube cbc;
  uint d;
  Node *parent;

public:
  void expand();
};
