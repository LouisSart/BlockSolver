#include "block.hpp"

Block<1, 3> b222({DLB}, {DL, DB, LB}, "222");
Block<1, 2> b223_ext({DLF}, {LF, DF}, "222_to_223");
Block<1, 2> xxcross_ext({DRB}, {DR, RB}, "223_to_f2l-1");
Block<0, 4> top_cross({}, {UF, UL, UR, UB}, "f2l-1_to_solved_edges");
BlockSolver<Block<1, 3>> b222_solver{b222};
BlockSolver<Block<1, 3>, Block<1, 2>> b223_solver{b222, b223_ext};
BlockSolver<Block<1, 3>, Block<1, 2>, Block<1, 2>> xxcross_solver{b222, b223_ext, xxcross_ext};
BlockSolver<Block<1, 3>, Block<1, 2>, Block<1, 2>, Block<0, 4>> edge_solver{b222, b223_ext, xxcross_ext, top_cross};