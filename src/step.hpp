#include <tuple>  // tables stored as tuples in Mover and Pruner

#include "coordinate_block_cube.hpp"  // MultiBlockCube
#include "move_table.hpp"             // BlockMoveTable
#include "pruning_table.hpp"          // load_ptr(Strategy)
#include "search.hpp"                 // DFS and IDA*

template <typename... MTs>
struct Mover {
    static constexpr unsigned _NTABLES = sizeof...(MTs);
    using Cube = MultiBlockCube<_NTABLES>;
    std::tuple<MTs*...> _mts;

    Mover() {}
    Mover(MTs*... mts) { _mts = std::tuple(mts...); }

    template <unsigned step>
    void apply(const Move& move, MultiBlockCube<_NTABLES>& mbc) const {
        static_assert(step < _NTABLES);
        std::get<step>(_mts)->apply(move, mbc[step]);
        if constexpr (step > 0) {
            apply<step - 1>(move, mbc);
        }
    }

    void apply(const Move& move, MultiBlockCube<_NTABLES>& mbc) const {
        apply<_NTABLES - 1>(move, mbc);
    }

    template <unsigned step>
    void apply(const Algorithm& seq, MultiBlockCube<_NTABLES>& mbc) const {
        static_assert(step < _NTABLES);
        std::get<step>(_mts)->apply(seq, mbc[step]);
        if constexpr (step > 0) {
            apply<step - 1>(seq, mbc);
        }
    }

    template <unsigned step>
    void apply_inverse(const Algorithm& seq,
                       MultiBlockCube<_NTABLES>& mbc) const {
        static_assert(step < _NTABLES);
        std::get<step>(_mts)->apply_inverse(seq, mbc[step]);
        if constexpr (step > 0) {
            apply_inverse<step - 1>(seq, mbc);
        }
    }

    void apply(const Algorithm& seq, MultiBlockCube<_NTABLES>& mbc) const {
        apply<_NTABLES - 1>(seq, mbc);
    }

    void apply_inverse(const Algorithm& seq,
                       MultiBlockCube<_NTABLES>& mbc) const {
        apply_inverse<_NTABLES - 1>(seq, mbc);
    }

    auto get_apply() const {
        return [this](const Move& move, MultiBlockCube<_NTABLES>& mbc) {
            this->apply<_NTABLES - 1>(move, mbc);
        };
    }
};

template <typename... BlockTypes>
auto make_mover(const BlockTypes&... blocks) {
    return Mover(new BlockMoveTable(blocks)...);
}

template <typename... BlockTypes>
auto make_eo_mover(const BlockTypes&... blocks) {
    return Mover(new EOMoveTable(), new BlockMoveTable(blocks)...);
}

template <typename... PTs>
struct Pruner {
    static constexpr unsigned _NTABLES = sizeof...(PTs);
    using Cube = MultiBlockCube<_NTABLES>;

    std::tuple<PTs*...> _pts;

    Pruner(PTs*... pts) { _pts = std::tuple(pts...); }

    template <unsigned block>
    unsigned get_estimate(const Cube& state) const {
        // Return the pruning value for block number `block`
        static_assert(block < _NTABLES);
        return std::get<block>(_pts)->get_estimate(state[block]);
    };

    template <unsigned current, unsigned next, unsigned... more>
    unsigned get_estimate(const Cube& state) const {
        // Recursively return the max between pruning values for blocks
        // number `current`, `next`, and `more...`
        auto current_estimate = get_estimate<current>(state);
        auto next_estimate = get_estimate<next, more...>(state);
        return current_estimate > next_estimate ? current_estimate
                                                : next_estimate;
    };

    template <unsigned... blocks>
    auto get_estimator() const {
        // Return a lambda that can compute the max
        // of all pruning values for all `blocks`
        return [this](const Cube& state) -> unsigned {
            return this->get_estimate<blocks...>(state);
        };
    }
};

template <typename... BlockTypes>
auto make_pruner(const BlockTypes&... blocks) {
    return Pruner(load_table_ptr(Strategy::Optimal(blocks))...);
}

template <typename... BlockTypes>
auto make_eo_pruner(const BlockTypes&... blocks) {
    return Pruner(load_table_ptr(Strategy::OptimalEO()),
                  load_table_ptr(Strategy::Optimal(blocks))...);
}

template <unsigned... steps, typename Cube>
bool is_solved(const Cube& cube) {
    return (cube[steps].is_solved() && ...);
}

template <unsigned... steps, typename Cube>
auto get_is_solved(const Cube& cube) {
    assert(sizeof...(steps) <= cube.size());
    return [](const Cube& cube) -> bool { return is_solved<steps...>(cube); };
}

template <typename NodePtr, typename Mover>
struct StepBase {
    Mover mover;
    StepBase() {}
    virtual Solutions<NodePtr> solve_optimal(NodePtr root,
                                             unsigned max_depth = 20,
                                             unsigned slackness = 0) = 0;
    virtual ~StepBase() = default;
};

template <typename NodePtr, typename Mover, typename Pruner, unsigned... steps>
struct Step : StepBase<NodePtr, Mover> {
    using Cube = typename Pruner::Cube;
    Pruner pruner;
    Mover mover;

    Step(const Mover& mover, const Pruner& pruner)
        : mover{mover}, pruner{pruner} {};

    Solutions<NodePtr> solve_optimal(NodePtr root, unsigned max_depth = 20,
                                     unsigned slackness = 0) {
        auto roots = Solutions<NodePtr>({root});
        auto node_sols = IDAstar<false>(
            roots, mover.get_apply(), pruner.template get_estimator<steps...>(),
            get_is_solved<steps...>(root->state), max_depth, slackness);
        node_sols.sort_by_depth();
        for (auto node : node_sols) {
            node->step_number++;
        }
        return node_sols;
    }
};

template <unsigned... steps, typename Mover, typename Pruner>
auto make_step(const Mover& mover, const Pruner& pruner) {
    using NodePtr = typename Node<typename Pruner::Cube>::sptr;
    return new Step<NodePtr, Mover, Pruner, steps...>(mover, pruner);
}

template <typename Mover, typename Pruner>
auto make_steps(std::initializer_list<StepBase<Mover, Pruner>*> args) {
    return std::vector<StepBase<Mover, Pruner>*>(args);
}

template <typename Mover>
auto init_roots(const Algorithm& scramble, std::vector<Algorithm> symmetries,
                const Mover& mover) {
    // Generates all symmetries of the scramble as
    // MultiBlockCubes<steps>
    // state = S^-1 * M * S, where S is the symmetry (rotation) and M is
    // the
    // scramble
    using Cube = typename Mover::Cube;
    using NodePtr = typename Node<Cube>::sptr;
    Solutions<NodePtr> ret;
    Cube cube;
    NodePtr root;
    for (auto sym : symmetries) {
        cube.set_solved();
        mover.apply_inverse(sym, cube);
        mover.apply(scramble, cube);
        mover.apply(sym, cube);
        root = make_root(cube);
        root->last_moves = sym;
        ret.push_back(root);
    }
    return ret;
}