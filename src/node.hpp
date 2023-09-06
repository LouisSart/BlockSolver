#pragma once
#include "algorithm.hpp"
#include <vector>
#include <iostream>

template<typename Cube>
struct Node
{
  Cube cube;
  int depth;

  Node(): depth{0} {}
  Node(Cube c, int d): cube{c}, depth{d} {}

  template<typename F>
  std::vector<Cube> expand(F apply, std::vector<Move> directions) const {
    std::vector<Cube> children;

    for (auto&& move : directions) {
      Cube child;
      child = cube;
      apply(move, child);
      children.push_back(child);
    }
     return children;
  };

  void show() const {
    std::cout << "Node object: " << std::endl;
    std::cout << " Depth: " << depth << std::endl;
    std::cout << " Cube type: " << typeid(cube).name() << std::endl;
  }
};
