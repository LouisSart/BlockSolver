#pragma once
#include "algorithm.hpp"
#include <vector>
#include <iostream>

template<typename Cube>
struct Node
{
  Cube cube;
  int depth;
  Move previous_move;

  Node(): depth{0}, previous_move{NoneMove} {}
  Node(Cube c, int d): cube{c}, depth{d}, previous_move{NoneMove} {}
  Node(Cube c, int d, Move prev): cube{c}, depth{d}, previous_move{prev} {}

  template<typename F, typename MoveContainer>
  std::vector<Node<Cube>> expand(F apply, MoveContainer directions) const {
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
