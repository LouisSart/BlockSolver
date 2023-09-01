#pragma once
#include <vector>
#include <iostream>

enum Move{U, U2, U3, D, D2, D3, R, R2, R3, L, L2, L3, F, F2, F3, B, B2, B3};

struct Algorithm{
  std::vector<Move> sequence;

  Algorithm(){};
  Algorithm(std::vector<Move> s): sequence{s}{};
};
