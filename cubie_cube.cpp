#include "cubie_cube.hpp"
#include "algorithm.hpp"

void CubieCube::show() const // Une fonction qui affiche l'état d'un CubieCube
{
  auto print_array = [](std::string name, int size, const uint* arr)  // Une lambda pour ne pas avoir à réécrire 4 fois la boucle
  {
    std::cout << name << " {";
    for (int i = 0; i<size; i++)
    {
      std::cout << arr[i];
      if (i<size-1){std::cout << ",";}
    }
    std:: cout << "}" << std::endl;
  };

  // Affichage des quatre arrays qui définissent un CubieCube
  std::cout << "CubieCube object:" << '\n';
  std::cout << "  ";
  print_array("CP", 8, cp);
  std::cout << "  ";
  print_array("CO", 8, co);
  std::cout << "  ";
  print_array("EP", 12, ep);
  std::cout << "  ";
  print_array("EO", 12, eo);
}

void CubieCube::corner_apply(CubieCube move) // Application en place d'un move aux coins
{
  int new_cp[8], new_co[8];
  for (int i=0; i<8; i++)
  {
    new_cp[i] = cp[move.cp[i]];
    new_co[i] = co[move.cp[i]] + move.co[i];
  };
  for (int i = 0; i < 8; i++)
  {
    cp[i] = new_cp[i];
    co[i] = new_co[i]%3;
  }
}

void CubieCube::edge_apply(CubieCube move) // Application en place d'un move aux arêtes
{
  int new_ep[12], new_eo[12];
  for (int i=0; i<12; i++)
  {
    new_ep[i] = ep[move.ep[i]];
    new_eo[i] = eo[move.ep[i]] + move.eo[i];
  };
  for (int i = 0; i < 12; i++)
  {
    ep[i] = new_ep[i];
    eo[i] = new_eo[i]%2;
  }
}

void CubieCube::apply(std::string alg_str) // Application en place d'un algo aux coins et arêtes
{
  Algorithm alg(alg_str);
  for (size_t i = 0; i < alg.size(); i++)
  {
    this->edge_apply(moves[alg[i]]);
    this->corner_apply(moves[alg[i]]);
  }
}
