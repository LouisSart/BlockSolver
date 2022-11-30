#include <array>
#include "move_table.hpp"

struct CoordinateBlockCube
{
  uint ccl, cel, ccp, cep, cco, ceo ; // Coordinates for cp, co, ep, eo, corner layout and edge layout
  MoveTable *mt;

  CoordinateBlockCube(){};
  CoordinateBlockCube(MoveTable);
  void set(uint,uint,uint,uint,uint,uint);
  void twist(uint move);
  void apply(std::string);
  void show();
};
