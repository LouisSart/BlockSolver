#include <iostream>
#include <vector>

constexpr uint ipow(uint k, uint n)
{
  // integer power
  // computes ret = k^n
  uint ret{1};
  for (uint i=0;i<n;i++)
  {
    ret*=k;
  }
  return ret;
};

constexpr uint factorial(int n)
{
  // recursive impl of the factorial: n!
  if (n<0)
  {
    std::cout << "factorial error: n= " << n << '\n';
    return -1;
  }
  else if (n==0) {return 1;}
  else {return n*factorial(n-1);}
};
constexpr uint binomial(uint n, uint k)
{
  // binomial coefficient C_{n,k}
  if (n<k){return 0;}
  else {return factorial(n)/(factorial(k)*factorial(n-k));}

};
extern uint perm_coord(uint*, uint);
extern uint layout_coord(uint*, uint);
extern uint co_coord(uint*, uint);
extern uint eo_coord(uint*, uint);
extern void perm_from_coord(uint, uint, uint*);
extern void layout_from_coord(uint, uint, uint, uint*);
extern void co_from_coord(uint, uint, uint*);
extern void eo_from_coord(uint, uint, uint*);
extern void corner_move_table_size(uint);
extern void edge_move_table_size(uint);
