#pragma once
#include <vector>
#include <array>
#include <iostream>

enum Move{U, U2, U3, D, D2, D3, R, R2, R3, L, L2, L3, F, F2, F3, B, B2, B3};

struct Algorithm{
  std::vector<Move> sequence;

  Algorithm(){};
  Algorithm(std::vector<Move> s): sequence{s}{};
};

// 0:U 1:U2 2:U' 3:D 4:D2 5:D' 6:R 7:R2 8:R' 9:L 10:L2 11:L' 12:F 13:F2 14:F' 15:B 16:B2 17:B' 18:S_URF 19:S_URF2 20:z2 21:y 22:y2 23:y' 24:S_LR
std::array<Move, 18> HTM_Moves{U, U2, U3, D, D2, D3, R, R2, R3, L, L2, L3, F, F2, F3, B, B2, B3};