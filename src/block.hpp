#include <algorithm> // std::find
#include <limits> // std::numeric_limits
#include "../extern/EpiCube/333/cubie_cube.hpp"
#include "../extern/EpiCube/lib/pruning_table.hpp"
#include "../extern/EpiCube/lib/search.hpp"

using namespace Cube333;

static fs::path program_name = "block";
static fs::path data_dir = app_data_directory() / program_name;
static fs::path ptable_dir = data_dir / "ptables";

template<unsigned NC, unsigned NE>
struct Block {
    static constexpr unsigned CLCARD = Layout<Cube333::NC, NC>::CARD;
    static constexpr unsigned CPCARD = Permutation<NC>::CARD;
    static constexpr unsigned COCARD = Orientation<NC, 3>::CARD;
    static constexpr unsigned CCARD = CLCARD * CPCARD * COCARD;
    static constexpr unsigned ELCARD = Layout<Cube333::NE, NE>::CARD;
    static constexpr unsigned EPCARD = Permutation<NE>::CARD;
    static constexpr unsigned EOCARD = Orientation<NE>::CARD;
    static constexpr unsigned ECARD = ELCARD * EPCARD * EOCARD;
    static constexpr unsigned CARD = CCARD * ECARD;

    const std::array<unsigned, NC> corners;
    const std::array<unsigned, NE> edges;

    PruningTable<CARD> ptable;
    fs::path ptable_path;

    Block(const std::array<unsigned, NC> &c, const std::array<unsigned, NE> &e, const std::string bname = "") : corners{c}, edges{e} {
        static_assert(ECARD < std::numeric_limits<unsigned>::max() / CCARD); // do not overflow unsigned
        static_assert(CCARD < std::numeric_limits<unsigned>::max() / ECARD); // do not overflow unsigned
        static_assert(NC < Cube333::NC); // no full corner blocks because of constraints on CO
        static_assert(NE < Cube333::NE); // no full edge blocks because of constraints on EO

        if (bname.size() == 0) {
            ptable_path = ptable_dir / id();
        } else {ptable_path = ptable_dir / bname;}

        if (!ptable.load(ptable_path)){
            ptable.template generate<CubieCube, true>(
                [this](const CubieCube& cc){return index(cc);},
                [this](const unsigned &c, CubieCube& cc){from_index(c, cc);},
                HTM::moves, 3, 3 + NC + NE // weird tuning for switch depths, but it kinda works lol
            );
            ptable.write(ptable_path);
        }
    }

    unsigned corner_index(const CubieCube &cc) const {
        if constexpr (NC == 0) {
            return 0;
        } else {
            Layout<Cube333::NC, NC> l(cc.cp, corners);
            Permutation<NC> p = cc.cp.get_sub_permutation<NC>(corners);
            Orientation<NC, 3> o = cc.co.get_sub_orientation<NC>(l);
            return (l.index() * CPCARD + p.index()) * COCARD + o.index();
        }
    }

    unsigned edge_index(const CubieCube &cc) const {
        if constexpr (NE == 0) {
            return 0;
        } else {
            Layout<Cube333::NE, NE> l(cc.ep, edges);
            Permutation<NE> p = cc.ep.get_sub_permutation<NE>(edges);
            Orientation<NE> o = cc.eo.get_sub_orientation<NE>(l);
            return (l.index() * EPCARD + p.index()) * EOCARD + o.index();
        }
    }

    unsigned index(const CubieCube& cc) const {
        return corner_index(cc) * ECARD + edge_index(cc);
    }

    void corners_from_index(const unsigned &c, CubieCube& cc) const {
        if constexpr (NC > 0) {
            unsigned li = (c / COCARD) / CPCARD;
            unsigned pi = (c / COCARD) % CPCARD;
            unsigned oi = c % COCARD;

            Layout<Cube333::NC, NC> l(li);
            cc.cp.set_from_partial_index(pi, l, corners);
            cc.co.set_from_partial_index(oi, l);
        }
    }

    void edges_from_index(const unsigned &c, CubieCube& cc) const {
        if constexpr (NE > 0) {
            unsigned li = (c / EOCARD) / EPCARD;
            unsigned pi = (c / EOCARD) % EPCARD;
            unsigned oi = c % EOCARD;

            Layout<Cube333::NE, NE> l(li);
            cc.ep.set_from_partial_index(pi, l, edges);
            cc.eo.set_from_partial_index(oi, l);
        }
    }

    void from_index(const unsigned &c, CubieCube& cc) const {
        corners_from_index(c / ECARD, cc);
        edges_from_index(c % ECARD, cc);
    }

    auto is_solved(const CubieCube& cc) const {
        for (auto c : corners) {
            if (cc.cp[c] != c) return false;
            if (cc.co[c] != 0) return false;
        }
        for (auto e : edges) {
            if (cc.ep[e] != e) return false;
            if (cc.eo[e] != 0) return false;
        }
        return true;
    }

    unsigned estimate(const CubieCube& cc) const {
        return ptable.estimate(index(cc));
    }

    std::string id() const {
        CubieCube cc;
        Layout<Cube333::NC, NC> lc (cc.cp, corners);
        Layout<Cube333::NE, NE> le (cc.ep, edges);

        auto c_id = lc.index();
        auto e_id = le.index();

        // Block ID is of the form: ncCcid_neEeid
        // For example the DLB 2x2x2 has ID: 1C7_3E217
        return std::to_string(NC) + "C" + std::to_string(c_id) + "_" +
               std::to_string(NE) + "E" + std::to_string(e_id);
    }
};

template<typename... Blocks>
struct BlockSolver {
    std::tuple<Blocks...> blocks;

    BlockSolver(Blocks... b) : blocks{b...}{}

    unsigned estimate(const CubieCube& cc) const {
        return std::apply(
            [&](const auto&... block) {
                return std::max({block.estimate(cc)...});
            },
            blocks
        );
    }

    bool is_solved(const CubieCube& cc) const {
        return std::apply(
            [&](const auto&... block){
                return (... && block.is_solved(cc));
            }, blocks
        );
    }

    auto solve(const std::deque<Node<CubieCube>::sptr> &roots) const {
        return IDAstar<false, CubieCube>(roots,
            [&](const CubieCube &cc) -> unsigned {return estimate(cc);},
            [&](const CubieCube &cc) -> bool {return is_solved(cc);},
            expand, 30);
    }
};

template<typename... Solvers>
auto make_multistep_solver(Solvers... solvers) {
    return MultiStep<CubieCube, Solvers...>(solvers...);
}