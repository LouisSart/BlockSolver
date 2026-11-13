#include "block.hpp"

namespace solvers {

Block<1, 3> b222({DLB}, {DL, DB, LB}, "222");
Block<2, 4> f2lm1({DLF, DRB}, {LF, DF, DR, RB}, "222_to_f2l-1");
Block<0, 4> top_cross({}, {UF, UL, UR, UB}, "f2l-1_to_solved_edges");
B222 b222_solver{b222};
F2LM1 f2lm1_solver{b222, f2lm1};
EDGES edge_solver{b222, f2lm1, top_cross};
L3C l3c_solver{edge_solver};
MultiStep<CubieCube, B222, F2LM1, EDGES, L3C> three_step_l3c_solver = make_multistep_solver(b222_solver, f2lm1_solver, edge_solver, l3c_solver);

}; // namespace solvers

