#pragma once
#include <vector>
#include <array>
#include <iostream>

enum Move{U, U2, U3, D, D2, D3, R, R2, R3, L, L2, L3, F, F2, F3, B, B2, B3, NoneMove=-1};

struct Algorithm{
  std::vector<Move> sequence;

  Algorithm(){};
  Algorithm(std::vector<Move> s): sequence{s}{};

  void show();
};

std::array<std::string, 18> move_str{"U", "U2", "U'", "D", "D2", "D'", "R", "R2", "R'", "L", "L2", "L'", "F", "F2", "F'", "B", "B2", "B'"};
void Algorithm::show() {
  int size = 0;
  for (auto&& m : sequence) {
    if (m != NoneMove) {
      std::cout << move_str[m] << " ";
      size +=1;
    }
  }
  std::cout << "(" << size << ")" << std::endl;
}
// 0:U 1:U2 2:U' 3:D 4:D2 5:D' 6:R 7:R2 8:R' 9:L 10:L2 11:L' 12:F 13:F2 14:F' 15:B 16:B2 17:B' 18:S_URF 19:S_URF2 20:z2 21:y 22:y2 23:y' 24:S_LR
std::array<Move, 18> HTM_Moves{U, U2, U3, D, D2, D3, R, R2, R3, L, L2, L3, F, F2, F3, B, B2, B3};

std::vector<Move> allowed_next(const Move m) {
  // Utility function that takes in the last move applied to the current node 
  // and returns the set of all moves of different faces
  switch (m)
  {
  case U ... U3:
    return {D, D2, D3, R, R2, R3, L, L2, L3, F, F2, F3, B, B2, B3};
  case D ... D3:
    return {R, R2, R3, L, L2, L3, F, F2, F3, B, B2, B3};
  case R ... R3:
    return {U, U2, U3, D, D2, D3, L, L2, L3, F, F2, F3, B, B2, B3};
  case L ... L3:
    return {U, U2, U3, D, D2, D3, F, F2, F3, B, B2, B3};
  case F ... F3:
    return {U, U2, U3, D, D2, D3, R, R2, R3, L, L2, L3, B, B2, B3};
  case B ... B3:
    return {U, U2, U3, D, D2, D3, R, R2, R3, L, L2, L3};
  default:
    return {U, U2, U3, D, D2, D3, R, R2, R3, L, L2, L3, F, F2, F3, B, B2, B3};
  }
}