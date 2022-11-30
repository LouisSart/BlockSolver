#include "coordinate_block_cube.hpp"
#include <iostream>

CoordinateBlockCube::CoordinateBlockCube(MoveTable table)
{
  mt = &table;
}

void CoordinateBlockCube::apply(std::string alg_str)
{
  Algorithm alg(alg_str);
  for (size_t i = 0; i < alg.size(); i++)
  {
    this->twist(alg[i]);
  }
}

void CoordinateBlockCube::twist(uint move)
{
  std::tie(ccl, ccp) = mt->get_new_ccl_ccp(ccl, ccp, move);
  std::tie(cel, cep) = mt->get_new_cel_cep(cel, cep, move);
  cco = mt->get_new_cco(ccl, cco, move);
  ceo = mt->get_new_ceo(cel, ceo, move);
}

void CoordinateBlockCube::set(uint ccl_in, uint cel_in, uint ccp_in, uint cep_in, uint cco_in, uint ceo_in)
{
  ccl=ccl_in; cel=cel_in; ccp=ccp_in; cep=cep_in; cco=cco_in; ceo=ceo_in;
}

void CoordinateBlockCube::show()
{
  std::cout << "CoordinateBlockCube:\n";
  std::cout << " Corner layout coordinate: " << ccl << '\n';
  std::cout << " CP coordinate: " << ccp << '\n';
  std::cout << " CO coordinate: " << cco << '\n';
  std::cout << " Edge layout coordinate: " << cel << '\n';
  std::cout << " EP coordinate: " << cep << '\n';
  std::cout << " EO coordinate: " << ceo << '\n';
}
