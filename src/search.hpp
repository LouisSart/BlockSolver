#pragma once
#include <deque>

#include "algorithm.hpp"
#include "node.hpp"
#include "pruning_table.hpp"

template <typename Cube, typename F>
std::vector<Algorithm> breadth_first_search(const Node<Cube> &root, F &&apply,
                                            size_t max_depth = 4) {
    std::vector<Algorithm> all_solutions;
    std::deque<Node<CoordinateBlockCube>> queue = {root};
    auto node = queue.back();

    while (queue.size() != 0) {
        auto node = queue.back();
        if (node.state.is_solved()) {
            std::cout << "Solution found at depth : " << node.depth
                      << std::endl;
            Algorithm solution(node.sequence);
            all_solutions.push_back(solution);
            queue.pop_back();
        } else {
            queue.pop_back();
            if (node.depth < max_depth) {
                auto children =
                    node.expand(apply, allowed_next(node.sequence.back()));
                for (auto &&child : children) {
                    queue.push_front(child);
                }
            }
        }
        assert(queue.size() < 1000000);  // Avoiding memory flood
    }
    if (all_solutions.size() == 0) {
        std::cout << "No solution found" << std::endl;
    }
    return all_solutions;
}
template <typename Cube, typename MoveTable, typename PruningTable>
std::vector<Algorithm> depth_first_search(const Node<Cube> &root,
                                          const MoveTable &m_table,
                                          const PruningTable &p_table,
                                          const uint max_depth = 4) {
    auto apply = [&m_table](const Move &move, Cube &cube) {
        m_table.apply(move, cube);
    };
    auto heuristic = [&p_table](const Cube &cube) {
        return p_table.get_estimate(cube);
    };
    std::vector<Algorithm> all_solutions;
    std::deque<Node<Cube>> queue = {root};
    auto node = queue.back();
    int node_counter = 0;

    while (queue.size() > 0) {
        auto node = queue.back();
        ++node_counter;
        if (node.state.is_solved()) {
            Algorithm solution(node.sequence);
            all_solutions.push_back(solution);
            queue.pop_back();
        } else {
            queue.pop_back();
            if (node.depth + node.estimate <= max_depth) {
                auto children = node.expand(apply, heuristic,
                                            allowed_next(node.sequence.back()));
                for (auto &&child : children) {
                    queue.push_back(child);
                }
            }
        }
        assert(queue.size() < 1000000);  // Avoiding memory flood
    }
    if (all_solutions.size() == 0) {
        std::cout << "No solution found" << std::endl;
    }
    std::cout << "Nodes generated: " << node_counter << std::endl;
    return all_solutions;
}
