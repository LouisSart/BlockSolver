#include "block.hpp"

Block<1, 3> b222({DLB}, {DL, DB, LB}, "222");
Block<1, 2> b223_ext({DLF}, {LF, DF}, "222_to_223");
Block<1, 2> xxcross_ext({DRB}, {DR, RB}, "223_to_f2l-1");
Block<0, 4> top_cross({}, {UF, UL, UR, UB}, "f2l-1_to_solved_edges");
BlockSolver<Block<1, 3>> b222_solver{b222};
BlockSolver<Block<1, 3>, Block<1, 2>> b223_solver{b222, b223_ext};
BlockSolver<Block<1, 3>, Block<1, 2>, Block<1, 2>> xxcross_solver{b222, b223_ext, xxcross_ext};
BlockSolver<Block<1, 3>, Block<1, 2>, Block<1, 2>, Block<0, 4>> edge_solver{b222, b223_ext, xxcross_ext, top_cross};

L3CSolver::L3CSolver() : top_corners{{ULF, URF, URB, ULB}, {}, "f2l-1_to_l3c", true}{

    auto cycle = [](const std::vector<unsigned> pieces, CubieCube& cc) {
        std::swap(cc.cp[pieces[0]], cc.cp[pieces[1]]);
        std::swap(cc.cp[pieces[0]], cc.cp[pieces[2]]);
    };

    if (!top_corners.ptable.load(top_corners.ptable_path)){
        top_corners.ptable.reset();
        unsigned counter = 0;
        for (unsigned c = 0; c < Layout<5, 3>::CARD; ++c) {
            const std::vector<unsigned> l5c {ULF, URF, URB, ULB, DRF};
            const Layout<5, 3> l(c);
            std::vector<unsigned> pieces;
            for (unsigned i = 0; i < 5; ++i) {
                if (l[i] == 1)  pieces.push_back(l5c[i]);
            }

            CubieCube cc;
            for (unsigned _ : {0, 1}){
                cycle(pieces, cc); // cycle the three corners twice
                for (unsigned o1 = 0; o1 < 3; ++o1){
                    for (unsigned o2 = 0; o2 < 3; ++o2){
                        cc.co[pieces[0]] = o1;
                        cc.co[pieces[1]] = o2;
                        cc.co[pieces[2]] = (6 - o1 - o2) % 3;
                        top_corners.ptable.set(top_corners.index(cc), 0);
                        ++counter;
                    }
                }
            }
        }

        top_corners.ptable.template generate<CubieCube, true>(
            [this](const CubieCube& cc){return top_corners.index(cc);},
            [this](const unsigned &c, CubieCube& cc){top_corners.from_index(c, cc);},
            HTM::moves, 3, 4, 180 // 180 3-corner cycles are set to depth zero
        );
        top_corners.ptable.write(top_corners.ptable_path);
    }

}

bool L3CSolver::is_solved(const CubieCube& cc) const {
    return edge_solver.is_solved(cc) && (top_corners.ptable[top_corners.index(cc)] == 0);
}

unsigned L3CSolver::estimate(const CubieCube& cc) const {
    return std::max(edge_solver.estimate(cc), top_corners.ptable.estimate(top_corners.index(cc)));
}

Solutions<CubieCube> L3CSolver::solve(const Solutions<CubieCube> &roots) const {
    return IDAstar<false, CubieCube>(roots,
        [&](const CubieCube &cc) -> unsigned {return estimate(cc);},
        [&](const CubieCube &cc) -> bool {return is_solved(cc);},
        expand, 30);
}

L3CSolver l3c_solver;