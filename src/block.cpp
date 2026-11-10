#include <algorithm> // std::find
#include <limits> // std::numeric_limits
#include "../extern/EpiCube/333/cubie_cube.hpp"
#include "../extern/EpiCube/lib/pruning_table.hpp"
#include "../extern/EpiCube/lib/search.hpp"

using namespace Cube333;

template<unsigned NC, unsigned NE>
struct Block {
    static constexpr unsigned CLCARD = Layout<Cube333::NC, NC>::CARD;
    static constexpr unsigned CPCARD = Permutation<NC>::CARD;
    static constexpr unsigned COCARD = Orientation<NC>::CARD;
    static constexpr unsigned CCARD = CLCARD * CPCARD * COCARD;
    static constexpr unsigned ELCARD = Layout<Cube333::NE, NE>::CARD;
    static constexpr unsigned EPCARD = Permutation<NE>::CARD;
    static constexpr unsigned EOCARD = Orientation<NE>::CARD;
    static constexpr unsigned ECARD = ELCARD * EPCARD * EOCARD;
    static constexpr unsigned CARD = CCARD * ECARD;

    const std::array<unsigned, NC> corners;
    const std::array<unsigned, NE> edges;

    Block(const std::array<unsigned, NC> &c, const std::array<unsigned, NE> &e) : corners{c}, edges{e} {
        static_assert(ECARD < std::numeric_limits<unsigned>::max() / CCARD); // do not overflow unsigned
        static_assert(CCARD < std::numeric_limits<unsigned>::max() / ECARD); // do not overflow unsigned
        static_assert(NC < Cube333::NC); // no full corner blocks because of constraints on CO
        static_assert(NE < Cube333::NE); // no full edge blocks because of constraints on EO
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
        }
        for (auto e : edges) {
            if (cc.ep[e] != e) return false;
        }
        return true;
    }
};

struct CrossSolver {
    Block<0, 4> cross;
    PruningTable<Block<0, 4>::CARD> ptable;

    CrossSolver() : cross({}, {DF, DL, DB, DR}), ptable() {
        ptable.generate<CubieCube, true>(
            [this](const CubieCube& cc){return cross.index(cc);},
            [this](const unsigned &c, CubieCube& cc){cross.from_index(c, cc);},
            HTM::moves, 3, 6
        );
    }

    auto solve(const CubieCube& cc) {
        return IDAstar<false, CubieCube>(
            make_root(cc),
            [this](const CubieCube &cc) -> unsigned {return ptable.estimate(cross.index(cc));},
            [this](const CubieCube& cc) -> bool {return cross.is_solved(cc);},
            expand, 10);
    };
};

void test_partial() {
    CubieCube cc;
    Block<0, 4> cross({}, {DF, DL, DB, DR});
    Block<1, 3> b222 ({DLB}, {DL, DB, LB});
    Block<4, 0> top_corners ({ULF, URF, URB, ULB}, {});

    auto check_index = [](const auto& b) -> void {
        CubieCube cc;
        for (unsigned c = 0; c < b.CARD; ++c) {
            b.from_index(c, cc);
            // print(c, b.index(cc));
            assert(b.index(cc) == c);
        }
    };

    check_index(cross);
    check_index(b222);
    check_index(top_corners);
}

int main (){
    test_partial();
    CubieCube cc("R' U' F L U D F2 D' F B U F' R2 F2 U2 F2 D2 R' F2 L2 D2 B2 L R' U' F");
    CrossSolver().solve(cc).show<Move>();
	return 0;
}