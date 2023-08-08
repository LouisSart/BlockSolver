#pragma once
#include <array>
#include "block_cube.hpp"

struct CoordinateBlockCube
{
  uint ccl, cel, ccp, cep, cco, ceo ; // Coordinates for cp, co, ep, eo, corner layout and edge layout

  CoordinateBlockCube(){};
  template <uint nc, uint ne>
  CoordinateBlockCube(BlockCube<nc, ne> &bc){
    bc.to_coordinates(ccl, cel, ccp, cep, cco, ceo);
  };
  // void twist(uint move){
  //   std::tie(ccl, ccp) = mt->get_new_ccl_ccp(ccl, ccp, move);
  //   std::tie(cel, cep) = mt->get_new_cel_cep(cel, cep, move);
  //   cco = mt->get_new_cco(ccl, cco, move);
  //   ceo = mt->get_new_ceo(cel, ceo, move);
  // };
  // void apply(std::string){
  //   Algorithm alg(alg_str);
  //   for (size_t i = 0; i < alg.size(); i++)
  //     {
  //       this->twist(alg[i]);
  //     }
  // };
  void show() {
    std::cout << "CoordinateBlockCube:\n";
    std::cout << " Corner layout coordinate: " << ccl << '\n';
    std::cout << " CP coordinate: " << ccp << '\n';
    std::cout << " CO coordinate: " << cco << '\n';
    std::cout << " Edge layout coordinate: " << cel << '\n';
    std::cout << " EP coordinate: " << cep << '\n';
    std::cout << " EO coordinate: " << ceo << '\n';
};
};
