#pragma once
#include "algorithm.hpp"
#include <vector>
#include <iostream>

template<typename Cube>
struct Node
{
  Cube state;
  int depth;
  std::vector<Move> sequence; // The moves that were made to get there

  Node(): state{Cube()}, depth{0}, sequence{NoneMove} {}
  Node(Cube c, int d): state{c}, depth{d}, sequence{NoneMove} {}
  Node(Cube c, int d, std::vector<Move> seq): state{c}, depth{d}, sequence{seq} {}

  template<typename F, typename MoveContainer>
  std::vector<Node<Cube>> expand(F apply, MoveContainer directions) const {
    std::vector<Node<Cube>> children;
    Cube next;

    for (auto&& move : directions) {
      next = state;
      apply(move, next);
      auto extended_sequence = sequence;
      extended_sequence.push_back(move);
      children.push_back(Node(next, depth + 1, extended_sequence));
    }
    return children;
  };

  void show() const {
    std::cout << "Node object: " << std::endl;
    std::cout << " Depth: " << depth << std::endl;
    std::cout << " Cube type: " << typeid(state).name() << std::endl;
  }
};
