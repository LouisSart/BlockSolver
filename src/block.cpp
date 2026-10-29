#include <algorithm> // std::find
#include <functional> // std::mem_fun
#include "../extern/EpiCube/333/cubie_cube.hpp"
#include "../extern/EpiCube/lib/pruning_table.hpp"
#include "../extern/EpiCube/lib/search.hpp"

using namespace Cube333;

constexpr std::array<unsigned, 4> edges {DF, DL, DB, DR};
constexpr unsigned LCARD = Layout<NE, 4>::CARD;
constexpr unsigned PCARD = Permutation<4>::CARD;
constexpr unsigned OCARD = Orientation<4>::CARD;
constexpr unsigned CARD = LCARD * PCARD * OCARD;

unsigned index(const CubieCube &cc) {
    Layout<NE, 4> l(cc.ep, edges);
    Permutation<4> p = cc.ep.get_sub_permutation<4>(edges);
    Orientation<4> o = cc.eo.get_sub_orientation<4>(l);

    return (l.index() * PCARD + p.index()) * OCARD + o.index();
}

void from_index(const unsigned &c, CubieCube& cc) {
    unsigned li = (c / OCARD) / PCARD;
    unsigned pi = (c / OCARD) % PCARD;
    unsigned oi = c % OCARD;

    Layout<NE, 4> l(li);
    cc.ep.set_from_partial_index(pi, l, edges);
    cc.eo.set_from_partial_index(oi, l);
}

void test_partial() {
    CubieCube cc;
    for (unsigned c = 0; c < CARD; ++c) {
        from_index(c, cc);
        assert(index(cc) == c);
    }
}

auto is_solved(const CubieCube& cc){
    for (auto e : edges) {
        if (cc.ep[e] != e) return false;
    }
    return true;
}

auto solve(const CubieCube& cc) {
    static PruningTable<CARD> ptable;
    ptable.generate<CubieCube, true>(index, from_index, HTM::moves, 3, 6);

    auto estimate = [](const CubieCube &cc) -> unsigned {return ptable.estimate(index(cc));};

    return IDAstar(make_root(cc), std::function(estimate), std::function(is_solved), expand, 10);
};

int main (){
    test_partial();
    CubieCube cc("R' U' F L U D F2 D' F B U F' R2 F2 U2 F2 D2 R' F2 L2 D2 B2 L R' U' F");
    solve(cc).show<Move>();;
	return 0;
}