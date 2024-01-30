#include <tuple>

#include "coordinate_block_cube.hpp"
#include "move_table.hpp"

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

    void apply(const Algorithm& seq, MultiBlockCube<_NTABLES>& mbc) const {
        apply<_NTABLES - 1>(seq, mbc);
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

    template <unsigned step>
    unsigned get_estimate(const MultiBlockCube<_NTABLES>& state) const {
        static_assert(step < _NTABLES);
        auto this_step_estimate =
            std::get<step>(_pts)->get_estimate(state[step]);
        if constexpr (step == 0) {
            return this_step_estimate;
        } else {
            return (this_step_estimate > get_estimate<step - 1>(state))
                       ? this_step_estimate
                       : get_estimate<step - 1>(state);
        }
    };

    template <unsigned steps>
    auto get_estimator() const {
        return [this](const MultiBlockCube<_NTABLES>& state) -> unsigned {
            return this->get_estimate<steps>(state);
        };
    }
};

template <typename Cube>
auto get_is_solved(const Cube& cube, const unsigned steps) {
    assert(steps < cube.size());
    return [steps](const Cube& cube) -> bool {
        for (unsigned k = 0; k <= steps; ++k) {
            if (!cube[k].is_solved()) {
                return false;
            }
        }
        return true;
    };
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
    void show() const {
        std::cout << "Step object:" << std::endl;
        std::cout << "   Step number: " << number << std::endl;
        std::cout << "   Depth: " << depth << std::endl;
        if (parent != nullptr) {
            std::cout << "    Step solution:" << std::endl;
            path.show();
        }
    }
};