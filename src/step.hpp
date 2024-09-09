#include <tuple>  // tables stored as tuples in Mover and Pruner

#include "coordinate_block_cube.hpp"  // MultiBlockCube
#include "move_table.hpp"             // BlockMoveTable
#include "pruning_table.hpp"          // load_ptr(Strategy)
#include "search.hpp"                 // DFS and IDA*

// template <typename... MTs>
// struct Mover {
//     static constexpr unsigned _NTABLES = sizeof...(MTs);
//     using Cube = MultiBlockCube<_NTABLES>;
//     std::tuple<MTs*...> _mts;

//     Mover() {}
//     Mover(MTs*... mts) { _mts = std::tuple(mts...); }

//     template <unsigned step>
//     void apply(const Move& move, MultiBlockCube<_NTABLES>& mbc) const {
//         static_assert(step < _NTABLES);
//         std::get<step>(_mts)->apply(move, mbc[step]);
//         if constexpr (step > 0) {
//             apply<step - 1>(move, mbc);
//         }
//     }

//     void apply(const Move& move, MultiBlockCube<_NTABLES>& mbc) const {
//         apply<_NTABLES - 1>(move, mbc);
//     }

//     template <unsigned step>
//     void apply(const Algorithm& seq, MultiBlockCube<_NTABLES>& mbc) const {
//         static_assert(step < _NTABLES);
//         std::get<step>(_mts)->apply(seq, mbc[step]);
//         if constexpr (step > 0) {
//             apply<step - 1>(seq, mbc);
//         }
//     }

//     template <unsigned step>
//     void apply_inverse(const Algorithm& seq,
//                        MultiBlockCube<_NTABLES>& mbc) const {
//         static_assert(step < _NTABLES);
//         std::get<step>(_mts)->apply_inverse(seq, mbc[step]);
//         if constexpr (step > 0) {
//             apply_inverse<step - 1>(seq, mbc);
//         }
//     }

//     void apply(const Algorithm& seq, MultiBlockCube<_NTABLES>& mbc) const {
//         apply<_NTABLES - 1>(seq, mbc);
//     }

//     void apply_inverse(const Algorithm& seq,
//                        MultiBlockCube<_NTABLES>& mbc) const {
//         apply_inverse<_NTABLES - 1>(seq, mbc);
//     }

//     auto get_apply() const {
//         return [this](const Move& move, MultiBlockCube<_NTABLES>& mbc) {
//             this->apply<_NTABLES - 1>(move, mbc);
//         };
//     }
// };

// template <typename... BlockTypes>
// auto make_mover(BlockTypes&... blocks) {
//     return Mover(new BlockMoveTable(blocks)...);
// }

// template <typename... BlockTypes>
// auto make_eo_mover(BlockTypes&... blocks) {
//     return Mover(new EOMoveTable(), new BlockMoveTable(blocks)...);
// }

// template <typename... PTs>
// struct Pruner {
//     static constexpr unsigned _NTABLES = sizeof...(PTs);
//     using Cube = MultiBlockCube<_NTABLES>;

//     std::tuple<PTs...> _pts;

//     Pruner(PTs... pts) { _pts = std::tuple(pts...); }

//     template <unsigned block>
//     unsigned get_estimate(const Cube& state) const {
//         // Return the pruning value for block number `block`
//         static_assert(block < _NTABLES);
//         return std::get<block>(_pts).get_estimate(state[block]);
//     };

//     template <unsigned current, unsigned next, unsigned... more>
//     unsigned get_estimate(const Cube& state) const {
//         // Recursively return the max between pruning values for blocks
//         // number `current`, `next`, and `more...`
//         auto current_estimate = get_estimate<current>(state);
//         auto next_estimate = get_estimate<next, more...>(state);
//         return current_estimate > next_estimate ? current_estimate
//                                                 : next_estimate;
//     };

//     template <unsigned... blocks>
//     auto get_estimator() const {
//         // Return a lambda that can compute the max
//         // of all pruning values for all `blocks`
//         return [this](const Cube& state) -> unsigned {
//             return this->get_estimate<blocks...>(state);
//         };
//     }
// };

// template <typename... BlockTypes>
// auto make_pruner(BlockTypes&... blocks) {
//     return Pruner(load_pruning_table(blocks)...);
// }

// template <typename... BlockTypes>
// auto make_eo_pruner(const BlockTypes&... blocks) {
//     return Pruner(load_eo_table(),
//                   load_pruning_table(Strategy::Optimal(blocks))...);
// }

template <std::size_t NB, typename MoveTable>
auto get_sym_apply(const MoveTable& m_table,
                   const std::array<unsigned, NB>& rotations) {
    return [&m_table, &rotations](const Move& move, MultiBlockCube<NB>& cube) {
        for (unsigned k = 0; k < NB; ++k) {
            m_table.sym_apply(move, rotations[k], cube[k]);
        }
    };
}

template <std::size_t NB, typename Block>
auto get_is_solved(Block& block) {
    return [&block](const MultiBlockCube<NB>& cube) {
        // Returns true if at least one of the cbc is solved
        for (auto cbc : cube) {
            if (block.is_solved(cbc)) return true;
        }
        return false;
    };
}

template <std::size_t NB, typename PruningTable>
auto get_estimator(const PruningTable& p_table) {
    return [&p_table](const MultiBlockCube<NB>& cube) {
        // Return the minimum estimate over all the cbcs
        unsigned ret = p_table.get_estimate(cube[0]);
        for (auto cbc : cube) {
            auto e = p_table.get_estimate(cbc);
            ret = (e < ret) ? e : ret;
        }
        return ret;
    };
}

// template <typename NodePtr, typename Mover>
// struct StepBase {
//     Mover mover;
//     StepBase() {}
//     virtual Solutions<NodePtr> solve_optimal(NodePtr root,
//                                              unsigned max_depth = 20,
//                                              unsigned slackness = 0) = 0;
//     virtual ~StepBase() = default;
// };

// template <typename NodePtr, typename Mover, typename Pruner, unsigned...
// steps> struct Step : StepBase<NodePtr, Mover> {
//     using Cube = typename Pruner::Cube;
//     Pruner pruner;
//     Mover mover;

//     Step(const Mover& mover, const Pruner& pruner)
//         : mover{mover}, pruner{pruner} {};

//     Solutions<NodePtr> solve_optimal(NodePtr root, unsigned max_depth = 20,
//                                      unsigned slackness = 0) {
//         auto roots = Solutions<NodePtr>({root});
//         auto node_sols = IDAstar<false>(
//             roots, mover.get_apply(), pruner.template
//             get_estimator<steps...>(), get_is_solved<steps...>(root->state),
//             max_depth, slackness);
//         node_sols.sort_by_depth();
//         for (auto node : node_sols) {
//             node->step_number++;
//         }
//         return node_sols;
//     }
// };

// template <unsigned... steps, typename Mover, typename Pruner>
// auto make_step(const Mover& mover, const Pruner& pruner) {
//     using NodePtr = typename Node<typename Pruner::Cube>::sptr;
//     return new Step<NodePtr, Mover, Pruner, steps...>(mover, pruner);
// }

// template <typename Mover, typename Pruner>
// auto make_steps(std::initializer_list<StepBase<Mover, Pruner>*> args) {
//     return std::vector<StepBase<Mover, Pruner>*>(args);
// }

template <typename Block, std::size_t NB>
auto init_root(const Algorithm& scramble, Block& block,
               const std::array<unsigned, NB> rotations) {
    CubieCube scramble_cc(scramble);
    MultiBlockCube<NB> ret;

    for (unsigned k = 0; k < NB; ++k) {
        ret[k] = block.to_coordinate_block_cube(
            scramble_cc.get_conjugate(rotations[k]));
    }
    return make_root(ret);
}