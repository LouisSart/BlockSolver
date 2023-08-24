#pragma once
#include <array>

struct CoordinateBlockCube
{
  uint ccl, cel, ccp, cep, cco, ceo ; // Coordinates for cp, co, ep, eo, corner layout and edge layout

  CoordinateBlockCube(){};
  void set(uint ccl, uint cel, uint ccp, uint cep, uint cco, uint ceo){
    ccl=ccl; cel=cel; ccp=ccp; cep=cep; cco=cco; ceo=ceo;
  };

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
