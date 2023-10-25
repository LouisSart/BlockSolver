#include <vector>

#include "algorithm.hpp"
#include "move_table.hpp"
#include "node.hpp"
#include "pruning_table.hpp"
#include "search.hpp"

struct StepNode {
    std::vector<Algorithm> solutions;
    std::vector<Algorithm> setups;
    unsigned depth{0};
    CubieCube state;

    StepNode(const Algorithm& scramble) { state = CubieCube(scramble); }
    StepNode(const StepNode& parent, const Algorithm& setup) {
        *this = parent;
        setups.push_back(setup);
        state = CubieCube();
        state.apply(CubieCube(setup).get_inverse());
        state.apply(parent.state);
        state.apply(CubieCube(setup));
    }
    StepNode(const StepNode& parent, const Algorithm& solution,
             const unsigned& d) {
        *this = parent;
        solutions.push_back(solution);
        depth += d;
        state.apply(solution);
    }

    void show() const {
        assert(solutions.size() == setups.size());
        std::cout << "StepNode object" << std::endl;
        for (unsigned k = 0; k < setups.size(); ++k) {
            Algorithm step_sequence = setups[k];
            step_sequence.concatenate(solutions[k]);
            step_sequence.show();
        }
    }
};

struct StepSolutions : public std::vector<StepNode> {
    void concatenate(const StepSolutions& other) {
        for (const StepNode& node : other) {
            push_back(node);
        }
    }
    void show() const {
        for (const auto& node : *this) {
            node.show();
        }
    }
};

template <typename Strategy>
struct Step {
    Algorithm scramble;
    Strategy strat;
    std::vector<Algorithm> setups;
    unsigned max_depth = 20, step_length = 20;

    Step(const Algorithm& sc, const Strategy& st,
         const std::vector<Algorithm>& se)
        : scramble{sc}, strat{st}, setups{se} {}
    Step(const Algorithm& sc, const Strategy& st,
         const std::vector<Algorithm>& se, const unsigned& md,
         const unsigned& sl)
        : scramble{sc}, strat{st}, setups{se}, max_depth{md}, step_length{sl} {}

    void show() const {
        scramble.show();
        std::cout << "Step Object:" << std::endl;
        strat.show();
        std::cout << "Max search depth: " << max_depth << std::endl;
        std::cout << "Max step length: " << step_length << std::endl;
    }

    StepSolutions setup_expand(const StepNode& parent) const {
        StepSolutions ret;
        for (const auto& setup : setups) {
            ret.emplace_back(parent, setup);
        }
        return ret;
    }

    unsigned search_depth(const StepNode& node) const {
        return (step_length - node.depth < max_depth)
                   ? max_depth
                   : step_length - node.depth;
    };

    template <typename PTable, typename MTable>
    StepSolutions solve_expand(const StepNode& parent, const PTable& p_table,
                               const MTable& m_table) const {
        BlockCube bc(strat.block);
        auto cbc = bc.to_coordinate_block_cube(parent.state);
        Node<CoordinateBlockCube> root(cbc, 0);
        unsigned srch_dpth = search_depth(parent);
        std::vector<Algorithm> solutions =
            IDAstar<false>(root, m_table, p_table, srch_dpth);

        StepSolutions ret;
        for (const auto& sol : solutions) {
            ret.emplace_back(parent, sol, sol.size());
        }
        return ret;
    }

    template <typename PTable, typename MTable>
    StepSolutions expand(const StepNode& parent, const PTable& p_table,
                         const MTable& m_table) const {
        StepSolutions ret;
        StepSolutions setup_nodes = setup_expand(parent);

        for (const auto& setup_node : setup_nodes) {
            StepSolutions this_setup_solutions =
                solve_expand(setup_node, p_table, m_table);
            ret.concatenate(this_setup_solutions);
        }
        return ret;
    }

    StepSolutions expand(const StepSolutions& prev_step_solutions) {
        PruningTable p_table = strat.load_table();
        BlockMoveTable m_table(strat.block);

        StepSolutions ret;
        for (const auto& node : prev_step_solutions) {
            ret.concatenate(expand(node, p_table, m_table));
        }
        return ret;
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
