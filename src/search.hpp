#pragma once
#include <cassert>
#include <deque>

#include "algorithm.hpp"
#include "node.hpp"

using Solutions = std::vector<Algorithm>;

template <typename NodePtr>
auto standard_directions(const NodePtr node) {
    if (node->parent == nullptr) {
        return default_directions;
    } else {
        return allowed_next(node->last_move);
    }
}

template <typename SolutionContainer>
void show(SolutionContainer solutions) {
    for (auto &&s : solutions) {
        s.show();
    }
}
template <typename NodePtr, typename F, typename SolveCheck>
Solutions breadth_first_search(const NodePtr root, F &&apply,
                               SolveCheck &&is_solved,
                               const unsigned max_depth = 4) {
    Solutions all_solutions;
    std::deque<NodePtr> queue = {root};
    auto node = queue.back();

    while (queue.size() != 0) {
        auto node = queue.back();
        if (is_solved(node->state)) {
            all_solutions.push_back(node->get_path());
            queue.pop_back();
        } else {
            queue.pop_back();
            if (node->depth < max_depth) {
                auto children = node->expand(apply, standard_directions(node));
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
Solutions depth_first_search(const NodePtr root, const Mover &apply,
                             const Pruner &estimate,
                             const SolveCheck &is_solved,
                             const unsigned max_depth = 4) {
    Solutions all_solutions;
    std::deque<NodePtr> queue = {root};
    auto node = queue.back();
    int node_counter = 0;

    while (queue.size() > 0) {
        auto node = queue.back();
        ++node_counter;
        if (is_solved(node->state)) {
            all_solutions.push_back(node->get_path());
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
Solutions IDAstar(const NodePtr root, const Mover &apply,
                  const Pruner &estimate, const SolveCheck &is_solved,
                  const unsigned max_depth = 20) {
    auto search_depth = estimate(root->state);
    Solutions solutions;

    while (solutions.size() == 0 && search_depth <= max_depth) {
        if constexpr (verbose) {
            std::cout << "Searching at depth " << search_depth << std::endl;
        }
        solutions = depth_first_search<verbose>(root, apply, estimate,
                                                is_solved, search_depth);
        ++search_depth;
    }
    if constexpr (verbose) {
        if (solutions.size() == 0) {
            std::cout << "IDA*: No solution found" << std::endl;
        }
    }
    return solutions;
}