#include <algorithm> // std::find
#include <limits> // std::numeric_limits
#include "../extern/EpiCube/333/cubie_cube.hpp"
#include "../extern/EpiCube/lib/pruning_table.hpp"
#include "../extern/EpiCube/lib/search.hpp"

using namespace Cube333;

static fs::path program_name = "epic";
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

    Block(const std::array<unsigned, NC> &c, const std::array<unsigned, NE> &e, const std::string bname = "", const bool no_build = false) : corners{c}, edges{e} {
        static_assert(ECARD < std::numeric_limits<unsigned>::max() / CCARD); // do not overflow unsigned
        static_assert(CCARD < std::numeric_limits<unsigned>::max() / ECARD); // do not overflow unsigned
        static_assert(NC < Cube333::NC); // no full corner blocks because of constraints on CO
        static_assert(NE < Cube333::NE); // no full edge blocks because of constraints on EO

        if (bname.size() == 0) {
            ptable_path = ptable_dir / id();
        } else {ptable_path = ptable_dir / bname;}

        if(!no_build){
            if (!ptable.load(ptable_path)){
                ptable.template generate<CubieCube, true>(
                    [this](const CubieCube& cc){return index(cc);},
                    [this](const unsigned &c, CubieCube& cc){from_index(c, cc);},
                    HTM::moves, 3, 3 + NC + NE // weird tuning for switch depths, but it kinda works lol
                );
                ptable.write(ptable_path);
            }
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

    auto solve(const Solutions<CubieCube> &roots) const {
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

template<typename EdgeSolver>
struct L3CSolver {

    static constexpr unsigned CPCARD = Permutation<Cube333::NC>::CARD;
    static constexpr unsigned COCARD = Orientation<Cube333::NC, 3>::CARD / 3;
    static constexpr unsigned CARD = CPCARD * COCARD;
    EdgeSolver edge_solver;
    PruningTable<CARD> ptable;
    fs::path ptable_path = ptable_dir / "corners_to_l3c";

    L3CSolver(const EdgeSolver& es) : edge_solver{es} {
        auto cycle = [](const std::vector<unsigned> pieces, CubieCube& cc) {
            std::swap(cc.cp[pieces[0]], cc.cp[pieces[1]]);
            std::swap(cc.cp[pieces[0]], cc.cp[pieces[2]]);
        };

        if (!ptable.load(ptable_path)){
            ptable.reset();
            ptable.set(index(CubieCube()), 0); // set solved state to 0
            unsigned counter = 1;
            for (unsigned c = 0; c < Layout<5, 3>::CARD; ++c) {
                const std::vector<unsigned> l5c {ULF, URF, URB, ULB, DRF};
                const Layout<5, 3> l(c);
                std::vector<unsigned> pieces;
                for (unsigned i = 0; i < 5; ++i) {
                    if (l[i] == 1)  pieces.push_back(l5c[i]);
                }
                CubieCube cc;
                for (unsigned _ : {0, 1}){  // cycle the three corners twice
                    cycle(pieces, cc);
                    for (unsigned o1 = 0; o1 < 3; ++o1){ // loop through every possible orientation
                        for (unsigned o2 = 0; o2 < 3; ++o2){
                            cc.co[pieces[0]] = o1;
                            cc.co[pieces[1]] = o2;
                            cc.co[pieces[2]] = (6 - o1 - o2) % 3;
                            ptable.set(index(cc), 0);
                            ++counter;
                        }
                    }
                }
            }

            ptable.template generate<CubieCube, true>(
                [this](const CubieCube& cc){return index(cc);},
                [this](const unsigned &c, CubieCube& cc){from_index(c, cc);},
                HTM::moves, 3, 10, counter // 180 3-corner cycles + the solved state are set to depth zero
            );
            ptable.write(ptable_path);
        }

    }

    unsigned index(const CubieCube& cc) const{
        return cc.cp.index() * COCARD + cc.co.index(true);
    }

    void from_index(const unsigned& c, CubieCube& cc) const{
        cc.cp.set_from_index(c / COCARD);
        cc.co.set_from_index(c % COCARD, true);
    }

    unsigned estimate(const CubieCube& cc) const{
        return std::max(edge_solver.estimate(cc), ptable.estimate(index(cc)));
    }

    bool is_solved(const CubieCube& cc) const{
        return edge_solver.is_solved(cc) && (ptable[index(cc)] == 0);
    }

    Solutions<CubieCube> solve(const Solutions<CubieCube>& roots) const{
        return IDAstar<false, CubieCube>(roots,
            [&](const CubieCube &cc) -> unsigned {return estimate(cc);},
            [&](const CubieCube &cc) -> bool {return is_solved(cc);},
            expand, 30);
    }
};

namespace solvers {

extern Block<1, 3> b222;
extern Block<2, 4> f2lm1;
extern Block<0, 4> top_cross;
using B222 = BlockSolver<Block<1, 3>>;
using F2LM1 = BlockSolver<Block<1, 3>, Block<2, 4>>;
using EDGES = BlockSolver<Block<1, 3>, Block<2, 4>, Block<0, 4>>;
using L3C = L3CSolver<EDGES>;

extern B222 b222_solver;
extern F2LM1 f2lm1_solver;
extern EDGES edge_solver;
extern L3C l3c_solver;
extern MultiStep<CubieCube, B222, F2LM1, EDGES, L3C> three_step_l3c_solver;

} // namespace solvers