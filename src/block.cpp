#include <algorithm> // std::find
#include <functional> // std::mem_fun
#include "../extern/EpiCube/333/cubie_cube.hpp"
#include "../extern/EpiCube/lib/pruning_table.hpp"
#include "../extern/EpiCube/lib/search.hpp"

using namespace Cube333;

std::array<unsigned, 4> edges {DF, DL, DB, DR};

unsigned index(const CubieCube &cc) {
    return cc.ep.partial_index(edges);
}

void from_index(const unsigned &c, CubieCube& cc) {
    cc.ep.set_from_partial_index(c, edges);
}

auto is_solved(const CubieCube& cc){
    for (auto e : edges) {
        if (cc.ep[e] != e) return false;
    }
    return true;
}

auto solve(const CubieCube& cc) {
    static PruningTable<11880> ptable;
    ptable.generate<CubieCube>(index, from_index, HTM::moves, 3, 6);

    auto estimate = [](const CubieCube &cc) -> unsigned {return ptable.estimate(index(cc));};

    return IDAstar(make_root(cc), std::function(estimate), std::function(is_solved), expand, 10);
};

int main (){
    CubieCube cc("D R L F2");
    solve(cc).show<Move>();;
	return 0;
}