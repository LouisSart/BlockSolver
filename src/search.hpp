#pragma once
#include <algorithm>
#include <cassert>
#include <deque>

#include "algorithm.hpp"
#include "node.hpp"

template <typename NodePtr>
struct Solutions : public std::vector<NodePtr> {
    void sort_by_depth() {
        std::sort(this->begin(), this->end(),
                  [](const NodePtr node1, const NodePtr node2) {
                      return (node1->depth < node2->depth);
                  });
    }
};

template <typename NodePtr>
auto standard_directions(const NodePtr node) {
    if (node->parent == nullptr) {
        return default_directions;
    } else {
        return allowed_next(node->last_moves.back());
    }
}

template <typename SolutionContainer>
void show(SolutionContainer solutions) {
    for (auto &&node : solutions) {
        node->get_path().show();
    }
}

template <typename NodePtr, typename Mover, typename Pruner,
          typename SolveCheck>
Solutions<NodePtr> breadth_first_search(const NodePtr root, Mover &&apply,
                                        const Pruner &&estimate,
                                        SolveCheck &&is_solved,
                                        const unsigned max_depth = 4) {
    Solutions<NodePtr> all_solutions;
    std::deque<NodePtr> queue = {root};
    auto node = queue.back();

    while (queue.size() != 0) {
        auto node = queue.back();
        if (is_solved(node->state)) {
            all_solutions.push_back(node);
            queue.pop_back();
        } else {
            queue.pop_back();
            if (node->depth < max_depth) {
                auto children =
                    node->expand(apply, estimate, standard_directions(node));
                for (auto &&child : children) {
                    queue.push_front(child);
                }
            }
        }
        assert(queue.size() < 1000000);  // Avoiding memory flood
    }
    return all_solutions;
}
template <bool verbose = true, typename NodePtr, typename Mover,
          typename Pruner, typename SolveCheck>
Solutions<NodePtr> depth_first_search(const NodePtr root, const Mover &apply,
                                      const Pruner &estimate,
                                      const SolveCheck &is_solved,
                                      const unsigned max_depth = 4) {
    Solutions<NodePtr> all_solutions;
    root->estimate = estimate(root->state);
    std::deque<NodePtr> queue = {root};
    auto node = queue.back();
    int node_counter = 0;

    while (queue.size() > 0) {
        auto node = queue.back();
        ++node_counter;
        if (is_solved(node->state)) {
            all_solutions.push_back(node);
            queue.pop_back();
        } else {
            queue.pop_back();
            if (node->depth + node->estimate <= max_depth) {
                auto children =
                    node->expand(apply, estimate, standard_directions(node));
                for (auto &&child : children) {
                    queue.push_back(child);
                }
            }
        }
        assert(queue.size() < 1000000);  // Avoiding memory flood
    }
    if constexpr (verbose) {
        std::cout << "Nodes generated: " << node_counter << std::endl;
    }
    return all_solutions;
}

template <bool verbose = true, typename NodePtr, typename Mover,
          typename Pruner, typename SolveCheck>
Solutions<NodePtr> IDAstar(const NodePtr root, const Mover &apply,
                           const Pruner &estimate, const SolveCheck &is_solved,
                           const unsigned max_depth = 20,
                           const unsigned slackness = 0) {
    auto search_depth = estimate(root->state);
    Solutions<NodePtr> solutions;

    while (solutions.size() == 0 && search_depth <= max_depth) {
        if constexpr (verbose) {
            std::cout << "Searching at depth " << search_depth << std::endl;
        }
        solutions = depth_first_search<verbose>(root, apply, estimate,
                                                is_solved, search_depth);
        ++search_depth;
    }
    if (slackness > 0) {
        // Find suboptimal with up to `slackness` extra moves
        // This implies that optimal solutions have been found
        // hence the job for those solutions is done twice.
        // I don' think this can be avoided since we need to
        // know optimal to introduce slackness
        solutions = depth_first_search<verbose>(
            root, apply, estimate, is_solved, search_depth + slackness - 1);
    }
    if constexpr (verbose) {
        if (solutions.size() == 0) {
            std::cout << "IDA*: No solution found" << std::endl;
        }
    }
    return solutions;
}

template <bool verbose = true, typename NodePtr, typename Mover,
          typename Pruner, typename SolveCheck>
Solutions<NodePtr> IDAstar(const Solutions<NodePtr> &roots, const Mover &apply,
                           const Pruner &estimate, const SolveCheck &is_solved,
                           const unsigned max_depth = 20) {
    // A version of IDAstar which returns the optimal
    // solutions over a set of different roots
    // Is used to find the optimal block given
    // various initial rotations of the cube
    Solutions<NodePtr> solutions;
    auto search_depth = max_depth;
    for (auto root : roots) {
        auto tmp =
            IDAstar<verbose>(root, apply, estimate, is_solved, search_depth);
        if (tmp.size() > 0) {
            // Update max_depth if solutions for current node are
            // shorter
            if (tmp[0]->depth < search_depth) {
                solutions.clear();
                search_depth = tmp[0]->depth;
            }
        }
        solutions.insert(solutions.end(), tmp.begin(), tmp.end());
    }
    return solutions;
}
