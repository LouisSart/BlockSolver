#include <vector>
#include <iostream>

struct Algorithm
{
  std::vector<uint> sequence;
  Algorithm(std::string);
  uint operator[](uint i){return sequence[i];};
  size_t size(){return sequence.size();};
};
