#include <sstream>
#include <map>
#include "algorithm.hpp"

const std::map<std::string, uint> move_string_to_int
{{"U", 0}, {"U2", 1}, {"U'", 2},
 {"D", 3}, {"D2", 4}, {"D'", 5},
 {"R", 6}, {"R2", 7}, {"R'", 8},
 {"L", 9}, {"L2", 10}, {"L'", 11},
 {"F", 12}, {"F2", 13}, {"F'", 14},
 {"B", 15}, {"B2", 16}, {"B'", 17}};

Algorithm::Algorithm(std::string str_alg)
{
  std::string cur_move="";
  std::stringstream ss(str_alg);

  while (std::getline(ss, cur_move, ' '))
  {
    sequence.push_back(move_string_to_int.at(cur_move));
  }
}
