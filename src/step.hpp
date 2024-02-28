#include <tuple>

#include "coordinate_block_cube.hpp"
#include "move_table.hpp"
#include "search.hpp"

template <typename... MTs>
struct Mover {
    static constexpr unsigned _NTABLES = sizeof...(MTs);
    std::tuple<MTs*...> _mts;

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

template <typename... PTs>
struct Pruner {
    static constexpr unsigned _NTABLES = sizeof...(PTs);
    std::tuple<PTs*...> _pts;

    Pruner(PTs*... pts) { _pts = std::tuple(pts...); }

    template <unsigned block>
    unsigned get_estimate(const MultiBlockCube<_NTABLES>& state) const {
        // Return the pruning value for block number `block`
        static_assert(block < _NTABLES);
        return std::get<block>(_pts)->get_estimate(state[block]);
    };

    template <unsigned current, unsigned next, unsigned... more>
    unsigned get_estimate(const MultiBlockCube<_NTABLES>& state) const {
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
        return [this](const MultiBlockCube<_NTABLES>& state) -> unsigned {
            return this->get_estimate<blocks...>(state);
        };
    }
};

template <unsigned... steps, typename Cube>
bool is_solved(const Cube& cube) {
    return (cube[steps].is_solved() && ...);
}

template <unsigned... steps, typename Cube>
auto get_is_solved(const Cube& cube) {
    static_assert(sizeof...(steps) <= cube.size());
    return [](const Cube& cube) -> bool { return is_solved<steps...>(cube); };
}

template <typename Cube>
struct StepNode : public std::enable_shared_from_this<StepNode<Cube>> {
    using sptr = std::shared_ptr<StepNode>;

    Cube state;
    sptr parent = nullptr;
    unsigned depth = 0;
    unsigned estimate = 0;
    Algorithm path;
    unsigned number = 0;

    static sptr make_root(const Cube& cube) {
        return sptr(new StepNode(cube, 0));
    }
    static sptr make_node(const Cube& cube, const unsigned& depth) {
        return sptr(new StepNode(cube, depth));
    }

   private:
    StepNode() : state{Cube()}, depth{0}, parent{nullptr}, estimate{0} {}
    StepNode(const Cube& c, const unsigned& d = 0, sptr p = nullptr,
             const Algorithm a = {}, const unsigned& e = 1)
        : state{c}, depth{d}, parent{p}, estimate{e} {
        if (p != nullptr) {
            path = a;
        }
    }

   public:
    std::vector<Algorithm> get_skeleton() const {
        std::vector<Algorithm> skeleton;
        Algorithm pth = path;
        skeleton.push_back(pth);
        sptr p = parent;
        while (p != nullptr) {
            pth = p->path;
            skeleton.push_back(pth);
            p = p->parent;
        }
        std::reverse(skeleton.begin(), skeleton.end());
        return skeleton;
    }

    void show() const {
        std::cout << "Step object:" << std::endl;
        std::cout << "   Step number: " << number << std::endl;
        std::cout << "   Depth: " << depth << std::endl;
        std::cout << "   Estimate: " << estimate << std::endl;
        if (parent != nullptr) {
            std::cout << "    Step solution:" << std::endl;
            path.show();
        }
    }
};

template <unsigned _NBLOCKS>
struct Method {
    using MultiCube = MultiBlockCube<_NBLOCKS>;
    using StepMultiNode = StepNode<MultiCube>;
    using StepNodePtr = StepMultiNode::sptr;
    using StepSolutions = Solutions<StepNodePtr>;

    template <typename Mover>
    StepSolutions init_roots(const Algorithm& scramble,
                             std::vector<Algorithm> symmetries,
                             const Mover& mover) {
        // Generates all symmetries of the scramble as MultiBlockCubes<blocks>
        // state = S^-1 * M * S, where S is the symmetry (rotation) and M is the
        // scramble
        StepSolutions ret;
        MultiBlockCube<_NBLOCKS> cube;
        StepNodePtr root;
        for (auto sym : symmetries) {
            cube.set_solved();
            mover.apply_inverse(sym, cube);
            mover.apply(scramble, cube);
            mover.apply(sym, cube);
            root = StepMultiNode::make_root(cube);
            root->path = Algorithm({sym});
            ret.push_back(root);
        }
        return ret;
    }

    // template <unsigned... blocks, typename Mover, typename Pruner>
    // auto make_step(const MultiNode::sptr root, unsigned step_depth,
    //                const Mover& mover, const Pruner& pruner) {
    //     return depth_first_search<false>(
    //         root, mover.get_apply(), pruner.template get_estimator<step>(),
    //         get_is_solved(root->state, step), step_depth);
    // }

    // template <typename Mover, typename Pruner, typename Splits>
    // auto expand(const StepNodePtr step_node_ptr, const Mover& mover,
    //             const Pruner& pruner, const Splits& splits) {
    //     NodePtr node =
    //         MultiNode::make_node(step_node_ptr->state, step_node_ptr->depth);
    //     Solutions<NodePtr> solutions;
    //     assert(step_node_ptr->number < 3);  // FIXME: any number of steps ?
    //     switch (step_node_ptr->number) {
    //         case 0:
    //             solutions = make_step<0>(node, splits[0], mover, pruner);
    //             break;
    //         case 1:
    //             solutions = make_step<1>(node, splits[1], mover, pruner);
    //             break;
    //         case 2:
    //             solutions = make_step<2>(node, splits[2], mover, pruner);
    //             break;
    //         default:
    //             std::cout << "Error in StepNode expansion" << std::endl;
    //             abort();
    //     }
    //     Solutions<StepNodePtr> ret;
    //     for (auto&& node_ptr : solutions) {
    //         auto child = StepMultiNode::make_node(node_ptr->state,
    //         node_ptr->depth); child->parent = step_node_ptr; child->path =
    //         node_ptr->get_path(); child->number = step_node_ptr->number + 1;
    //         ret.push_back(child);
    //     }
    //     return ret;
    // }
};