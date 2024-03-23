#pragma once
#include <iostream>
#include <memory>  // shared_ptr
#include <vector>  // expand() return type

#include "algorithm.hpp"

template <typename Cube>
struct Node : public std::enable_shared_from_this<Node<Cube>> {
    using sptr = std::shared_ptr<Node<Cube>>;
    using csptr = std::shared_ptr<Node<Cube> const>;
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
    bool is_root() const { return parent == nullptr; }

    template <typename F, typename H, typename MoveContainer>
    std::vector<sptr> expand(const F &apply, const H &heuristic,
                             const MoveContainer &directions) {
        // Generates the children nodes and computes their estimate using the
        // heuristic function
        std::vector<sptr> children;
        Cube next;
        for (auto &&move : directions) {
            next = state;
            apply(move, next);
            children.emplace_back(new Node(next, depth + 1,
                                           this->shared_from_this(), move,
                                           heuristic(next)));
        }
        return children;
    };

    Algorithm get_path() const {
        Algorithm path;
        csptr p = this->shared_from_this();
        while (p->parent != nullptr) {
            path.append(p->last_move);
            p = p->parent;
        }
        std::reverse(path.sequence.begin(), path.sequence.end());
        return path;
    }

    Algorithm get_sub_path(sptr ancestor) const {
        Algorithm path;
        csptr p = this->shared_from_this();
        while (p->parent != ancestor) {
            if (ancestor != nullptr) {
                assert(p->parent != nullptr);  // wrong ancestor ?
            }
            path.append(p->last_move);
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
