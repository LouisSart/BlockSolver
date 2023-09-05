#pragma once
#include <array>

struct CoordinateBlockCube
{
  uint ccl, cel, ccp, cep, cco, ceo ; // Coordinates for cp, co, ep, eo, corner layout and edge layout

  CoordinateBlockCube(){
    set(0., 0., 0., 0., 0., 0.);
  };
  void set(uint ccl_in, uint cel_in, uint ccp_in, uint cep_in, uint cco_in, uint ceo_in){
    ccl=ccl_in; cel=cel_in; ccp=ccp_in; cep=cep_in; cco=cco_in; ceo=ceo_in;
  };

  bool operator==(const CoordinateBlockCube &other){
    return (ccl == other.ccl 
          && cel == other.cel
          && ccp == other.ccp
          && cep == other.cep
          && cco == other.cco
          && ceo == other.ceo);
  }

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
