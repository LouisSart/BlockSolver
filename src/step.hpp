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