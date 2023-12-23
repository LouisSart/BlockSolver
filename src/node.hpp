#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "algorithm.hpp"

template <typename Cube>
struct Node : public std::enable_shared_from_this<Node<Cube>> {
    using sptr = std::shared_ptr<Node<Cube>>;
    Cube state;         // The cube state this node corresponds to
    sptr parent;        // The shared_ptr to the parent
    unsigned depth;     // The number of moves made to get this state
    unsigned estimate;  // The lower-bound estimate on the number of moves
                        // needed to solve the state
    Move last_move;     // The move which yielded this state

    static sptr make_root(const Cube &cube) { return sptr(new Node(cube, 0)); }
    static sptr make_node(const Cube &cube, const unsigned &depth) {
        return sptr(new Node(cube, depth));
    }

   private:
    Node() : state{Cube()}, depth{0}, parent{nullptr}, estimate{0} {}
    Node(const Cube &c, const unsigned &d = 0, sptr p = nullptr,
         const Move m = U, const unsigned &e = 1)
        : state{c}, depth{d}, parent{p}, estimate{e} {
        if (p != nullptr) {
            last_move = m;
        }
    }

   public:
    // template <int sequence_generation = 1, typename F, typename
    // MoveContainer> std::vector<sptr> expand(const F &apply, const
    // MoveContainer &directions) {
    //     // Generates the children nodes by using the apply function on all
    //     moves
    //     // in the directions container
    //     std::vector<sptr> children;
    //     Cube next;

    //     for (auto &&move : directions) {
    //         next = state;
    //         apply(move, next);
    //         children.emplace_back(new Node(next, depth + 1,
    //                                        this->shared_from_this(), move,
    //                                        heuristic(state)));
    //     }
    //     return children;
    // }

    template <typename F, typename H, typename MoveContainer>
    std::vector<sptr> expand(const F &apply, const H &heuristic,
                             const MoveContainer &directions) {
        // Generates the children nodes, computes their estimate using the
        // heuristic function and then sorts them by decreasing pruning values
        // (so that children.back() is the child with the lowest estimate.)
        std::vector<sptr> children;
        Cube next;
        for (auto &&move : directions) {
            next = state;
            apply(move, next);
            children.emplace_back(new Node(next, depth + 1,
                                           this->shared_from_this(), move,
                                           heuristic(state)));
        }
        std::sort(children.begin(), children.end(), [](sptr node1, sptr node2) {
            return node1->estimate > node2->estimate;
        });
        return children;
    };

    Algorithm get_path() const {
        Algorithm path;
        Move move = last_move;
        sptr p = parent;
        while (p != nullptr) {
            path.append(move);
            move = p->last_move;
            p = p->parent;
        }
        std::reverse(path.sequence.begin(), path.sequence.end());
        return path;
    }

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