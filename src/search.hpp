#include <deque>
#include "node.hpp"
#include "algorithm.hpp"

template<typename Cube, typename F>
std::vector<Algorithm> breadth_first_search(const Node<Cube> &root, F &&apply, size_t max_depth = 4) {

    std::vector<Algorithm> all_solutions;
    std::deque<Node<CoordinateBlockCube>> queue = {root};
    auto node = queue.back();

    while (queue.size() != 0) {
        auto node = queue.back();
        if (node.state.is_solved()) { 
            std::cout << "Solution found at depth : " << node.depth << std::endl;
            Algorithm solution(node.sequence);
            all_solutions.push_back(solution);
            queue.pop_back();
        }
        else {
            queue.pop_back();
            if (node.depth < max_depth){
                auto children = node.expand(
                    apply, allowed_next(node.sequence.back())
                );
                for (auto&& child : children) {
                    queue.push_front(child);
                }
            }
        }
        assert(queue.size() < 1000000); // Avoiding memory flood
    }
    if (all_solutions.size() == 0) {
        std::cout << "No solution found" << std::endl;
    }
    return all_solutions;
}
