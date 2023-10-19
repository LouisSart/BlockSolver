#pragma once
#include <algorithm>
#include <iostream>
#include <vector>

#include "algorithm.hpp"

template <typename Cube>
struct Node {
    Cube state;
    Algorithm path;  // The moves that were made to get there
    unsigned depth;
    unsigned estimate;  // The estimate on the number of moves needed to solve
                        // the state

    Node() : state{Cube()}, depth{0}, path{}, estimate{0} {}
    Node(const Cube &c, const unsigned &d)
        : state{c}, depth{d}, path{}, estimate{0} {}
    Node(const Cube &c, const unsigned &d, const std::vector<Move> &seq)
        : state{c}, depth{d}, path{seq}, estimate{0} {}

    template <int sequence_generation = 1, typename F, typename MoveContainer>
    std::vector<Node<Cube>> expand(const F &apply,
                                   const MoveContainer &directions) const {
        // Generates the children nodes by using the apply function on all moves
        // in the directions container
        std::vector<Node<Cube>> children;
        Cube next;

        for (auto &&move : directions) {
            next = state;
            apply(move, next);
            children.push_back(Node(next, depth + 1));
            if constexpr (sequence_generation > 0) {
                children.back().path = path;
                children.back().path.append(move);
            }
        }
        return children;
    }

    template <typename F, typename H, typename MoveContainer>
    std::vector<Node<Cube>> expand(const F &apply, const H &heuristic,
                                   const MoveContainer &directions) const {
        // Generates the children nodes, computes their estimate using the
        // heuristic function and then sorts them by decreasing pruning values
        // (so that children.back() is the child with the lowest estimate.)
        auto children = expand(apply, directions);
        for (auto &child : children) {
            child.estimate = heuristic(child.state);
        }
        std::sort(children.begin(), children.end(),
                  [](Node<Cube> node1, Node<Cube> node2) {
                      return node1.estimate > node2.estimate;
                  });
        return children;
    };

    void show() const {
        std::cout << "Node object: " << std::endl;
        std::cout << " Depth: " << depth << std::endl;
        std::cout << " Cube type: " << typeid(state).name() << std::endl;
        std::cout << " Pruning value: " << estimate << std::endl;
    }
};

template <typename index_type>
struct CompressedNode {
    index_type state;  // single-digit representation of a cube state
    uint8_t depth;

    CompressedNode() : state{0}, depth{0} {}
    CompressedNode(index_type state, uint8_t depth)
        : state{state}, depth{depth} {}
};
