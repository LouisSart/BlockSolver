# BlockSolver
A Rubik's Cube block solver

### Configuration ###

cmake -B build

### Compilation ###

cmake --build build

### Run script ###

There are three main scripts that you may want to run: three_step_solve, 223_solve and F2L-1_solve. Usage:

./SCRIPT_PATH [OPTIONS] SCRAMBLE

223_solve and F2L-1_solve respectively solve the optimal 2x2x3 block and F2L-1 block of the given scramble. All possible rotations of the cube are generated before starting the search. For example:

./build/script/223_solve "R L U D F B"

should return a bunch of 5 move solutions. Same for F2L-1_solve:

./build/script/F2L-1_solve "R L U D F B"

Note: On startup you will be prompted to generate a few tables. This can take a few minutes and is necessary for the program to run.

three_step_solve finds a corner skeleton in three steps: first solve the DLB 2x2x2, then extend it to an F2L-1 and finally solve the top cross edges to leave 5 corners. The search depth at each step is required, you can pass it using the -s`i` options, where i is the step index starting at 0. 


./build/script/three_step_solve -s0 6 -s1 13 -s2 18 "R L U D R2 L D F2 U' R' D2 L F' R2 F B R D F R' U2 L' F R2 F"


This example will find 5 corner skeletons starting with a 2x2x2 in 6 moves or fewer, followed by an F2L-1 in 13 or fewer, and a leave corner skeleton in 18 or better. The given threshold at each step is the total length for all steps cumulated, e.g. -s1 13 here means 2x2x2 + F2L-1 in 13 or fewer moves.

# Goal #

The (not so clear) goal that I have is to make an FMC helper tool to find human-findable skeletons of all kinds. At first I was just hoping to get a computer implementation of a 'leave 3C solver' but it could become a 'custom solver' allowing the user to define their own methods. I am hoping that this program could give us amazing solutions that are humanly understandable. Imagine an optimal Roux, or blockino solver...

# Limitations #

The program is still under development, and I think there are quite a few things that can be done to make it really amazing for FMC solvers and cube enjoyers.

### NISS ###

For now the program doesn't understand or use NISS, which make pretty poor at finding good block solutions. Definitely one of the things I have to wrap my head around.

### EO and DR ###

Even if this program doesn't have the goal of becoming a Thistleswaithe or Kociemba solver, I'd like to combine solving blocks and solving domino (blockino solving fashion). As for EO it is a must have even for block FMC solvers. Solving EO or DR could be done at any stage of the method, so I'll have to think of a dynamic order of the steps.

### More ###

I have a lot of ideas for the program, which are mostly impossible because I haven't the skill to do it, but I'll put them here in case somebody feels like they could do it. First is parallelism. I'm pretty sure the computations that are run here could be made way faster with some multithreading magic. I don't want to make a super powerful solver, but some speed up would improve the user experience.

Also maybe this code shouldn't do the impementation AND the application. I think turning buttons on and off, or adding more and more command line options isn't the way to go. I've thought of making it a Python library, but maybe just having input scripts to define your method and parameters is better.

# CubieCube #
A struct defining 4 arrays for cp, co, ep and eo

# BlockCube #
The block representation of a CubieCube

cl: bit array of where the corners are
cp: array of the corner permutation
co: array of the corner orientations
el: bit array of where the edges are
ep: array of the edge permutation
eo: array of the edge orientations

Example for a 1-corner and 3-edge block:
cl: [0, 0, 0, 1, 0, 0, 0, 0]
cp: [0]
co: [1]
el: [1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0]
ep: [0, 2, 1]
eo: [1, 1, 0]

Each of these states can be translated to a single integer called a coordinate.
Each coordinate ranges from 0 to the number of possible states and can be used as an index in a lookup table

# CoordinateBlockCube #
The coordinate representation of a subgroup of the cube pieces
(for example the three edges and one corner of a 2x2x2 block)
There are six coordinates that represent the state of the cubies:

ccl: the positions of the corners regardless of their order
ccp: the permutation of the corners
cco: the orientation of the corners
cel: the positions of the edges regardless of their order
cep: the permutation of the edges
ceo: the orientation of the edges


# Move Tables #

TODO

# Pruning Tables #

Pruning tables are used to accelerate the search for solutions. Basically the table stores the results of a function h (called a heuristic) such that for any CoordinateBlockCube cbc, h(cbc) is a lower-bound estimate of the optimal solution length for state cbc. This estimate can be used to prune the search tree as it is not necessary to search in a sub-tree whose root has a high estimate.

More concretely if you are looking for solutions under a given depth threshold max_depth, and if after m moves you run into a state with estimate h such that m + h > max_depth, then you know you can avoid generating the children of that state because they will not lead you to a solution that satisfies the criterion max_depth. The closer the estimate is to the optimal solution length for the state, the more nodes the search algorithm will be able to prune.

Usually the better pruning values require a lot of memory because they take into account the multiple characteristics of each state (layout, permutation and orientation). As there are many possible combinations of those characteristics, the number of entries in the table is higher. On the other hand a more 'lazy' heuristic table (that only takes into account the layout of the pieces for instance) will have a lower memory footprint but will store values that are further away from the optimal solution length on average. Which pruning value is best is mainly a tradeoff between RAM usage and mean value.

Pruning tables are generated using a breadth-first algorithm from the solved state. It iteratively generates all nodes at depth 1, then 2 ... Each newly encountered position is converted to a coordinate integer, and the depth at which it was found is written into an array. This array is then used as a lookup table by the search algorithms. As we get into deeper parts of the tree, it gets more and more rare to find a position that hasn't been visited yet and the generator slows down. To accelerate the generation algorithm, a backwards generator is used. The backwards generator starts from a not-yet-encoutered state S and generates its children. If one of them is a known position C, then depth(S) = depth(C) + 1. The table can thus be filled from the outer parts of the tree. The backwards generator cannot be used alone because at the beginning only the solved position is known at depth 0, and most of the unknown positions aren't connected with it. In this program the backwards search is triggered when most of the table is already set and only the outer layers of the tree are left to visit.

### Optimal Pruning Value ###

This is the optimal pruning value. It is exactly equal to the optimal solution length and allows the search algorithm to prune every sub-optimal path, leaving only those which are optimal. This makes the search optimally fast, as it allows to only generate the paths to the shortest solutions.

Pros: Finds the optimal path directly
Cons: Big table size, cannot be used for big blocks

### Permutation Pruning Value ###

This corresponds to the optimal number of moves needed to restore the block pieces to their home locations, without consideration for their respective orientations. This reduces the table size significantly, making it usable for about 10-11 pieces in the block.

Pros: Small table, usable for bigger blocks
Cons: The set of all estimate zero states is not the solved state alone, but a much bigger space. This means having a low PPV doesn't necessarily mean the state is close to solved. PPV is not good for low value states.

# Search Algorithms #

### Breadth-first search ###

Breadth-first search is the most naive algorithm for searching solution nodes in the search tree. Basically the algorithm applies every possible move to the scrambled state and stores the results as depth-one states. If some of them are solved states, then it retrieves the solutions. If not, then it goes through all depth-one states and applies every possible move to them and stores the depth-two states. It continues with increasing depth values until it finds a solution, or until a given depth limit is reached.

The main advantage of this method is that it theorically finds every solution, starting with the optimal ones. Its major downfall however is the RAM usage, because it needs to store every state at depth d when looking for solutions at depth d+1. The space size at depth d, given a number of possible moves m is O(m^d), making BFS unusable for medium to long solutions.

Pruning values can be used in BFS, but it is not implemented here.

### Depth-first search ###

Depth-first search uses a different strategy than BFS to avoid its memory limitations. Given a depth threshold max_depth, DFS iteratively applies one move to the scrambled state, then to the state that was just obtained, and continues with the children states until it reaches max_depth. This is why it is called depth-first. If no solution was found then DFS steps back to depth max_depth - 1 and applies the depth-first strategy up to max_depth again until there is no more nodes to generate. It then starts again from depth max-depth - 2 and so on until all nodes are visited or solutions have beed found.

DFS does not require as much memory because it only stores the path to one node at depth d (which is of size O(d)) when looking for solution at depth d+1. Its main downfall is that you need to have an estimate of the optimals length to set the max_depth parameter. If your estimate is too low then no solution will be found, and you will need to relaunch the search with a higher max_depth, and if it is too high then the algorithm will run for longer than necessary and find (possibly many) suboptimal solutions.

DFS can use a pruning value to reduce the search space size. To do so it applies the depth first strategy until it runs into a node at depth d such that d+h > max_depth, where h is the estimated distance of this node to the goal state. If h is an admissible heuristic (e.g. it never overestimates the distance to solved) then DFS will find every solution of length < max_depth. The closer the heuristic is to the optimal length, the faster DFS will find a solution.

### Iterative deepening search ###

IDA* needs a heuristic for better performance. It runs a depth-first search at depth h0 where h0 is the estimate for the root node. If no solution is found then IDA* uses the lowest value for max_depth + h that was found during the previous DFS as the new max_depth parameter. It continues with increasing values of max_depth until it reaches the optimal length and solutions are found.