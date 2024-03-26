#include <tuple>  // tables stored as tuples in Mover and Pruner

#include "coordinate_block_cube.hpp"  // MultiBlockCube
#include "move_table.hpp"             // BlockMoveTable
#include "pruning_table.hpp"          // load_ptr(Strategy)
#include "search.hpp"                 // DFS and IDA*

template <typename... MTs>
struct Mover {
    static constexpr unsigned _NTABLES = sizeof...(MTs);
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

template <typename NodePtr, typename Mover, typename Pruner, unsigned... blocks>
struct BlockStep : StepBase<NodePtr, Mover> {
    using Cube = typename Pruner::Cube;
    Pruner pruner;
    Mover mover;

    BlockStep(const Mover& mover, const Pruner& pruner)
        : mover{mover}, pruner{pruner} {};

    Solutions<NodePtr> solve_optimal(NodePtr root, unsigned max_depth = 20,
                                     unsigned slackness = 0) {
        auto roots = Solutions<NodePtr>({root});
        auto node_sols = IDAstar<false>(
            roots, mover.get_apply(),
            pruner.template get_estimator<blocks...>(),
            get_is_solved<blocks...>(root->state), max_depth, slackness);
        node_sols.sort_by_depth();
        for (auto node : node_sols) {
            node->step_number++;
        }
        return node_sols;
    }
};

template <unsigned... blocks, typename Mover, typename Pruner>
auto make_block_step(const Mover& mover, const Pruner& pruner) {
    using NodePtr = typename Node<typename Pruner::Cube>::sptr;
    return BlockStep<NodePtr, Mover, Pruner, blocks...>(mover, pruner);
}

template <typename Cube, typename Mover>
auto init_roots(const Algorithm& scramble, std::vector<Algorithm> symmetries,
                const Mover& mover) {
    // Generates all symmetries of the scramble as
    // MultiBlockCubes<blocks>
    // state = S^-1 * M * S, where S is the symmetry (rotation) and M is
    // the
    // scramble
    using NodePtr = Node<Cube>::sptr;
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

// template <typename Mover, typename Pruner>
// struct Method {
//     static constexpr unsigned _NBLOCKS = Mover::_NTABLES;
//     using MultiCube = MultiBlockCube<_NBLOCKS>;
//     using MultiNode = Node<MultiCube>;
//     using NodePtr = MultiNode::sptr;
//     using StepMultiNode = StepNode<MultiCube>;
//     using StepNodePtr = StepMultiNode::sptr;
//     using StepSolutions = Solutions<StepNodePtr>;

//     Mover mover;
//     Pruner pruner;

//     Method(Mover& m, Pruner& p) : pruner{p}, mover{m} {
//         static_assert(Pruner::_NTABLES == Mover::_NTABLES);
//     }

//     template <unsigned... blocks>
//     StepSolutions make_step(const StepNodePtr step_node_ptr,
//                             unsigned step_depth, const bool opt_only = false)
//                             {
//         NodePtr root =
//             MultiNode::make_node(step_node_ptr->state, step_node_ptr->depth);
//         Solutions<NodePtr> solutions;
//         if (opt_only) {  // Only find optimal solutions
//             solutions = IDAstar<false>(
//                 root, mover.get_apply(),
//                 pruner.template get_estimator<blocks...>(),
//                 get_is_solved<blocks...>(root->state), step_depth);
//         } else {  // Find all solutions that are shorter than max_depth
//             solutions = depth_first_search<false>(
//                 root, mover.get_apply(),
//                 pruner.template get_estimator<blocks...>(),
//                 get_is_solved<blocks...>(root->state), step_depth);
//         }

//         StepSolutions ret;
//         for (auto&& node_ptr : solutions) {
//             auto child =
//                 StepMultiNode::make_node(node_ptr->state, node_ptr->depth);
//             child->parent = step_node_ptr;
//             child->path = node_ptr->get_path();
//             ret.push_back(child);
//         }
//         return ret;
//     }

//     template <unsigned... blocks>
//     StepSolutions make_step(const StepSolutions& prev_step_solutions,
//                             unsigned step_depth, const bool opt_only = false)
//                             {
//         StepSolutions step_solutions;
//         auto max_depth = step_depth;
//         for (auto node : prev_step_solutions) {
//             auto tmp = make_step<blocks...>(node, max_depth, opt_only);
//             if (tmp.size() > 0) {
//                 if (opt_only) {
//                     // Update max_depth if solutions for current node are
//                     // shorter
//                     if (tmp[0]->depth < max_depth) {
//                         step_solutions.clear();
//                         max_depth = tmp[0]->depth;
//                     }
//                 }
//             }
//             step_solutions.insert(step_solutions.end(), tmp.begin(),
//             tmp.end());
//         }
//         return step_solutions;
//     }
// };

// template <typename... BlockTypes>
// auto make_method(const BlockTypes&... blocks) {
//     auto mover = Mover(new BlockMoveTable(blocks)...);
//     auto pruner = Pruner(load_table_ptr(Strategy::Optimal(blocks))...);
//     return Method(mover, pruner);
// }

// template <typename... BlockTypes>
// auto make_eo_method(const BlockTypes&... blocks) {
//     auto mover = Mover(new EOMoveTable(), new BlockMoveTable(blocks)...);
//     auto pruner = Pruner(load_table_ptr(Strategy::OptimalEO()),
//                          load_table_ptr(Strategy::Optimal(blocks))...);
//     return Method(mover, pruner);
// }

// constexpr bool OPT_ONLY = true;
// constexpr bool FIND_ALL = false;