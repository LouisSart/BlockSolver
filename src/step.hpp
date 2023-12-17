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
    unsigned h(const MultiBlockCube<_NTABLES>& state) const {
        static_assert(step < _NTABLES);
        if constexpr (step == 0) {
            return std::get<0>(_pts)->get_estimate(state[0]);
        } else {
            auto this_step_estimate =
                std::get<step>(_pts)->get_estimate(state[step]);
            return (this_step_estimate > h<step - 1>(state))
                       ? this_step_estimate
                       : h<step - 1>(state);
        }
    };

    unsigned get_estimate(const MultiBlockCube<_NTABLES>& state) const {
        return h<_NTABLES - 1>(state);
    }

    auto get_estimator() const {
        return [this](const MultiBlockCube<_NTABLES>& state) -> unsigned {
            return this->h<_NTABLES - 1>(state);
        };
    }
};

template <typename Cube>
auto get_is_solved(const Cube& cube) {
    return [](const Cube& cube) -> bool { return cube.is_solved(); };
}