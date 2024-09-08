#pragma once
#include <algorithm>
#include <cassert>
#include <deque>

#include "move.hpp"
#include "node.hpp"

template <typename NodePtr>
struct Solutions : public std::vector<NodePtr> {
    void sort_by_depth() {
        std::sort(this->begin(), this->end(),
                  [](const NodePtr node1, const NodePtr node2) {
                      return (node1->depth < node2->depth);
                  });
    }

    void show() const {
        for (auto node : *this) {
            node->get_path().show();
        }
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

template <bool verbose = true, typename NodePtr, typename Mover,
          typename Pruner, typename SolveCheck>
Solutions<NodePtr> depth_first_search(const NodePtr root, const Mover &apply,
                                      const Pruner &estimate,
                                      const SolveCheck &is_solved,
                                      const unsigned max_depth = 4) {
    Solutions<NodePtr> all_solutions;
    int node_counter = 0;
    std::deque<NodePtr> queue({root});

    while (queue.size() > 0) {
        auto node = queue.back();
        ++node_counter;
        if (is_solved(node->state)) {
            all_solutions.push_back(node);
            queue.pop_back();
        } else {
            queue.pop_back();
            if (node->depth + estimate(node->state) <= max_depth) {
                auto children = node->expand(apply, standard_directions(node));
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
    unsigned search_depth = estimate(root->state);

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
        auto depth = (max_depth < search_depth + slackness - 1)
                         ? max_depth
                         : search_depth + slackness - 1;
        solutions = depth_first_search<verbose>(root, apply, estimate,
                                                is_solved, depth);
    }
    if constexpr (verbose) {
        if (solutions.size() == 0) {
            std::cout << "IDA*: No solution found" << std::endl;
        }
    }
    return solutions;
}

// template <typename NodePtr, typename Phase>
// Solutions<NodePtr> multi_phase_search(const Solutions<NodePtr> roots,
//                                       const std::vector<Phase *> &phases,
//                                       const std::vector<unsigned> &splits,
//                                       unsigned slackness) {
//     assert(splits.size() == phases.size());
//     Solutions<NodePtr> all_solutions;
//     auto mover = phases[0]->mover;
//     std::deque<NodePtr> queue(roots.begin(), roots.end());
//     NodePtr node;

//     while (queue.size() > 0) {
//         node = queue.back();
//         queue.pop_back();
//         if (node->state.is_solved()) {
//             all_solutions.push_back(node);
//         } else {
//             if (node->step_number < phases.size()) {
//                 auto phase_solutions =
//                 phases[node->step_number]->solve_optimal(
//                     node, splits[node->step_number], slackness);
//                 queue.insert(queue.end(), phase_solutions.begin(),
//                              phase_solutions.end());
//             }
//         }
//     }
//     return all_solutions;
// }