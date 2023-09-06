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

  template<typename F, size_t metric_size>
  std::vector<Node<Cube>> expand(F apply, std::array<Move, metric_size> directions) const {
    std::vector<Node<Cube>> children;
    Cube child;

    for (auto&& move : directions) {
      child = cube;
      apply(move, child);
      children.push_back(Node(child, depth + 1));
    }

     return children;
  };

  void show() const {
    std::cout << "Node object: " << std::endl;
    std::cout << " Depth: " << depth << std::endl;
    std::cout << " Cube type: " << typeid(cube).name() << std::endl;
  }
};
