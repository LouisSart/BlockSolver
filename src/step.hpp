#include <vector>

#include "algorithm.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "search.hpp"

template <typename Strategy>
struct Step {
    using StepNode = Node<CoordinateBlockCube>;
    Strategy strat;
    std::vector<StepNode> solutions, start_up_nodes;
    std::vector<Algorithm> premoves;
    unsigned max_depth = 20, step_length = 20;

    Step(const Strategy& s) : strat{s} { strat.show(); }
    Step(const Strategy& s, const std::initializer_list<Algorithm>& setups)
        : strat{s}, premoves{setups} {}
    Step(const Strategy& s, const std::initializer_list<Algorithm>& setups,
         const unsigned& md, const unsigned& sl)
        : strat{s}, premoves{setups}, max_depth{md}, step_length{sl} {}

    void show() const {
        std::cout << "Step Object:" << std::endl;
        strat.show();
        std::cout << "Setups:" << std::endl;
        for (auto setup : premoves) {
            setup.show();
        }
        std::cout << "Max search depth: " << max_depth << std::endl;
        std::cout << "Max step length: " << step_length << std::endl;
    }

    void init(const std::vector<StepNode>& previous_step_nodes) {
        BlockMoveTable m_table(strat.block);
        for (const StepNode& node : previous_step_nodes) {
            start_up_nodes.push_back(node);
            for (auto setup : premoves) {
                auto setup_node = node;
                m_table.apply(setup, setup_node.state);
                setup_node.path.append(setup);
                start_up_nodes.push_back(setup_node);
            }
        }
    }

    void init(const Algorithm& scramble) {
        BlockMoveTable m_table(strat.block);
        StepNode root;
        m_table.apply(scramble, root.state);
        init({root});
    }
};

template <bool verbose = false, typename Strategy, typename Solutions>
auto step(const Algorithm& scramble, const Strategy& strat,
          const Solutions& prev_step_solutions, const unsigned& max_step_length,
          const unsigned& max_search_depth) {
    std::vector<Algorithm> solutions;
    auto p_table = PruningTable(strat);
    for (auto solution : prev_step_solutions) {
        Algorithm setup = scramble;
        setup.append(solution);
        unsigned search_depth =
            (max_step_length - solution.size() < max_search_depth)
                ? max_search_depth
                : max_step_length - solution.size();
        auto continuations =
            solve<verbose>(setup, p_table, strat, search_depth);
        for (auto continuation : continuations) {
            auto combination = solution;
            combination.append(continuation);
            solutions.push_back(combination);
        }
    }
    return solutions;
}

template <bool verbose = true, typename Strategy>
auto solve_with_setups(const Algorithm& scramble, const Strategy& strat,
                       const std::initializer_list<Move>& setups,
                       const unsigned& max_depth = 20) {
    std::vector<Algorithm> solutions;
    BlockMoveTable m_table(strat.block);
    CoordinateBlockCube cbc;

    for (auto setup : setups) {
        cbc.set(0, 0, 0, 0, 0, 0);
        m_table.apply_inverse(setup, cbc);
        m_table.apply(scramble, cbc);
        m_table.apply(setup, cbc);
        auto new_sols = solve<verbose>(cbc, strat, max_depth);
        for (const Algorithm sol : new_sols) {
            Algorithm sol_with_setup({setup});
            sol_with_setup.append(sol);
            solutions.push_back(sol_with_setup);
        }
    }
    return solutions;
}
