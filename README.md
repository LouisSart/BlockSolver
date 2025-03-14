# BlockSolver
This program can solve any of 4 different blocks (1x2x3, 2x2x2, 2x2x3 or F2L-1) on the input scramble.
It will find all optimal solutions over all possible symmetries of the cube.

It can also solve the F2L-1 suboptimally in three steps : 2x2x2, 2x2x3 and F2L-1 by switching before each step.

### Configuration ###

```console
cmake -B build
```

### Compilation ###

```console
cmake --build build
```

### Run script ###
Run using :

```console
./build/src/block_solver <step> <scramble>
```
where ```step``` can be any of ```123```, ```222```, ```223```, ```F2L-1```, ```multistep```

### Options ###

`-M`: maximum solution length. If optimal is shorter than `M` moves, then only optimals will be computed. Default `M = 15`  
`-b`: maximum number of solutions kept at each step (only for multistep solver). Use this parameter to reduce search time and memory usage or increase search breadth. Default `b = 500`  
`-s`: slackness of the optimal solver. When this parameter is set, the solver is allowed to use `s` more moves than optimal to produce solutions. Default `s=0` (optimal only)

Examples :

```console
epicier@w-Optiplex:~/BlockSolver$ ./build/src/block_solver 222 -M 6 "R L U D F B"
R L U D F B (6)
B' U' D' L' (4)
B' U' D' R' (4)
F' B' D' L' (4)
F' B' D' R' (4)
F' B' U' L' (4)
F' B' U' R' (4)
F' U' D' L' (4)
F' U' D' R' (4)
```

```console
epicier@w-Optiplex:~/BlockSolver$ ./build/src/block_solver F2L-1 "R' U' F L D F2 R2 D L2 U' L2 F2 U' F L2 D' F2 R' D' B2 U2 L' F2 R' U' F"
R' U' F L D F2 R2 D L2 U' L2 F2 U' F L2 D' F2 R' D' B2 U2 L' F2 R' U' F (26)
D2 B' U2 R2 B2 L F' L' F U (10)
```

```console
epicier@w-Optiplex:~/BlockSolver$ ./build/src/block_solver multistep -M 13 -b 1000 "R' U' F  D' B' U' D2 L' U2 L B2 R2 B2 U2 F' L B2 R' U' F"
R' U' F D' B' U' D2 L' U2 L B2 R2 B2 U2 F' L B2 R' U' F (20)
----------------
D' L' B2 U2 L2 // 2x2x2 (5/5)
(F2 R2 F U F2) // 2x2x3 (5/10)
(U2 R2 U') // F2L-1 (3/13)
----------------
D' L' B2 U2 L2 // 2x2x2 (5/5)
(F2 R2 F U F2) // 2x2x3 (5/10)
B U' B' // F2L-1 (3/13)
```
# Goal #

The purpose of this tool is to find human findable block skeletons. Optimal solutions to a 1x2x3, 2x2x2, 2x2x3 or F2L-1 can be found, as well as multi-step F2L-1 solutions using NISS before each step. Leave-3C-skeletons would be the natural next step.

### NISS ###

The multistep solver looks for linear solutions for each step on both inverse and normal.  
It is currently possible to input a scramble between brackets, but not a skeleton with subparts being on inverse.  
I don't plan on implementing a two sided search algorithm because it prevents the use of pruning tables, which slows down the algorithm by a lot.

### EO and DR ###

EO is implemented but is not yet usable. I don't think it is very interesting to have another EO>DR solver since there are already very good ones. See [nissy](https://nissy.tronto.net/), [cubelib](https://github.com/Jobarion/cubelib) and [Mallard](https://joba.me/mallard/) for human Thistlewaite solutions.
One application that I can see for those steps is blockino solving, but this is not the priority implementation.

### To-do list ###

- Optimal solvers : look for solutions on inverse
- Allow to look for suboptimal intermediate solutions in multistep solver

# Some documentation #
### CubieCube ###
A struct defining 4 arrays for cp, co, ep and eo

### Block ###
The block representation of a CubieCube

 + cl: bit array of where the corners are
 + cp: array of the corner permutation
 + co: array of the corner orientations
 + el: bit array of where the edges are
 + ep: array of the edge permutation
 + eo: array of the edge orientations

Example for a 1-corner and 3-edge block:
 + cl: [0, 0, 0, 1, 0, 0, 0, 0]
 + cp: [0]
 + co: [1]
 + el: [1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0]
 + ep: [0, 2, 1]
 + eo: [1, 1, 0]

Each of these states can be translated to a single integer called a coordinate.
Each coordinate ranges from 0 to the number of possible states minus one and can be used as an index in a lookup table. The edge permutation coordinate for the above CubieCube will range from 0 to 3! - 1

### CoordinateBlockCube ###

The coordinate representation of a subgroup of the cube pieces
(for example the three edges and one corner of a 2x2x2 block)
There are six coordinates that represent the state of the cubies:

 + ccl: the positions of the corners regardless of their order
 + ccp: the permutation of the corners
 + cco: the orientation of the corners
 + cel: the positions of the edges regardless of their order
 + cep: the permutation of the edges
 + ceo: the orientation of the edges


### Move Tables ###

Move tables are transition tables that store the result of applying each possible move to a given coordinate. This allows to perform moves faster that permuting digits in an array on the CubieCube level (the only cost is the lookup in the table). The move tables are precomputed at runtime and then written on the disk for later use.

### Pruning Tables ###

Pruning tables are used to accelerate the search for solutions. Basically the table stores the results of a function h (called a heuristic) such that for any CoordinateBlockCube cbc, h(cbc) is a lower-bound estimate of the optimal solution length for state cbc. This estimate can be used to prune the search tree as it is not necessary to search in a sub-tree whose root has a high estimate.

More concretely if you are looking for solutions under a given depth threshold max_depth, and if after m moves you run into a state with estimate h such that m + h > max_depth, then you know you can avoid generating the children of that state because they will not lead you to a solution that satisfies the criterion max_depth. The closer the estimate is to the optimal solution length for the state, the more nodes the search algorithm will be able to prune.

Usually the better pruning values require a lot of memory because they take into account the multiple characteristics of each state (layout, permutation and orientation). As there are many possible combinations of those characteristics, the number of entries in the table is higher. On the other hand a more 'lazy' heuristic table (that only takes into account the layout of the pieces for instance) will have a lower memory footprint but will store values that are further away from the optimal solution length on average. Which pruning value is best is mainly a tradeoff between RAM usage and mean value.

Pruning tables are generated using a breadth-first algorithm from the solved state. It iteratively generates all nodes at depth 1, then 2 ... Each newly encountered position is converted to a coordinate integer, and the depth at which it was found is written into an array. This array is then used as a lookup table by the search algorithms. As we get into deeper parts of the tree, it gets more and more rare to find a position that hasn't been visited yet and the generator slows down. To accelerate the generation algorithm, a backwards generator is used. The backwards generator starts from a not-yet-encoutered state S and generates its children. If one of them is a known position C, then depth(S) = depth(C) + 1. The table can thus be filled from the outer parts of the tree. The backwards generator cannot be used alone because at the beginning only the solved position is known at depth 0, and most of the unknown positions aren't connected with it. In this program the backwards search is triggered when most of the table is already set and only the outer layers of the tree are left to visit.

Similarly to move tables, pruning tables are generated when first needed, and then written on the disk for later use.

### Optimal Pruning Value ###

For small blocks like the 2x2x2 or 1x2x3, the length of the optimals can be stored with a limited memory cost (God's algorithm). It allows the search algorithm to prune every sub-optimal path, leaving only those which are optimal. This makes the search optimally fast, as it allows to only generate the paths to the shortest solutions.

### Split Pruning Value ###

For bigger blocks like the 2x2x3 or F2L-1, the table generation for God's algorithm can be impractical because of the table size. The solution that is implememented in this program is to split the block into smaller blocks for each of which the optimal pruning table is generated (it doesn't matter if these smaller blocks share some of the same pieces). The split pruning value to a given state is then the maximum value between the optimal pruning values for each block.

# Search Algorithms #

### Breadth-first search ###

Breadth-first search is the most naive algorithm for searching solution nodes in the search tree. Basically the algorithm applies every possible move to the scrambled state and stores the results as depth-one states. If some of them are solved states, then it retrieves the solutions. If not, then it goes through all depth-one states and applies every possible move to them and stores the depth-two states. It continues with increasing depth values until it finds a solution, or until a given depth limit is reached.

The main advantage of this method is that it theorically finds every solution, starting with the optimal ones. Its major downfall however is the RAM usage, because it needs to store every state at depth d when looking for solutions at depth d+1. The space size at depth d, given a number of possible moves m is O(m^d), making BFS unusable for medium to long solutions.

Pruning values can be used in BFS, but it is not implemented here.

### Depth-first search ###

Depth-first search uses a different strategy than BFS to avoid its memory limitations. Given a depth threshold max_depth, DFS iteratively applies one move to the scrambled state, then to the state that was just obtained, and continues with the children states until it reaches max_depth. This is why it is called depth-first. If no solution was found then DFS steps back to depth max_depth - 1 and applies the depth-first strategy up to max_depth again until there is no more nodes to generate. It then starts again from depth max_depth - 2 and so on until all nodes are visited or solutions have been found.

DFS does not require as much memory because it only stores the path to one node at depth d (which is of size O(d)) when looking for solution at depth d+1. Its main downfall is that you need to have an estimate of the optimals length to set the max_depth parameter. If your estimate is too low then no solution will be found, and you will need to run the search again with a higher max_depth, and if it is too high then the algorithm will run for longer than necessary and find (possibly many) suboptimal solutions in addition to the optimal ones.

DFS can use a pruning value to reduce the search space size. To do so it applies the depth first strategy until it runs into a node at depth d such that d+h > max_depth, where h is the estimated distance of this node to the goal state. If h is an admissible heuristic (e.g. it never overestimates the distance to solved) then DFS will find every solution of length < max_depth. The closer the heuristic is to the optimal length, the faster DFS will find a solution.

### Iterative deepening search ###

IDA* needs a heuristic for better performance. It runs a depth-first search at depth h0 where h0 is the estimate for the root node. If no solution is found then IDA* uses the lowest value for max_depth + h that was found during the previous DFS as the new max_depth parameter. It continues with increasing values of max_depth until it reaches the optimal length and solutions are found.