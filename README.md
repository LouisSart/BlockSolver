# BlockSolver
A Rubik's Cube block solver

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

Usually the better pruning values require a lot of memory because they take into account the multiple caracteristics of each state (layout, permutation and orientation). As there are many possible combinations of those caracteristics, the number of entries in the table is higher. On the other hand a more 'lazy' heuristic table (that only takes into account the layout of the pieces for instance) will have a lower memory footprint but will store values that are further away from the optimal solution length on average. Which pruning value is best is mainly a tradeoff between RAM usage and mean value.

TODO: Description of the algorithms for pruning table generation

### Exact Pruning Value ###

This is the optimal pruning value. It is exactly equal to the optimal solution length and allows the search algorithm to prune every sub-optimal path, leaving only those which are optimal. This makes the search optimally fast, as it allows to only generate the path to the shortest solutions. (Now that I write about this and use the word 'optimal' 5 times in 3 sentences, I think I should rename this the Optimal Pruning Value). 

Unfortunaltely it starts becoming unusable for high number of pieces in the block. For instance with 2 corners and 2 edges, the table has size 100 B, but for 5 corners and 5 edges you would need about 30 GB of RAM which I don't have (lol). So building a 2x2x2 or even a 2x2x3 block using an optimal pruning value would be feasible, but not the first two blocks of the Roux method at once. Furthermore the table generation requires some RAM. My experience is that a block of 4 edges and 4 corners (like the U layer for instance) requires too much RAM to be built on my machine, even though the table technically fits into my memory.

### Permutation Pruning Value ###

This corresponds to the optimal number of moves needed to restore the block pieces to their home locations, without consideration for their respective orientations. This reduces the table size significantly, making it usable for about 10-11 pieces in the block.

TODO: Average value study for this pruning value

### Orientation Pruning Value ###

Orientation Pruning value corresponds to the number of moves needed to restore the pieces to the block home location in the solved orientation, without consideration for their permutation among the block's cubie locations. The table size for this pruning value is smaller than the permutation pruning value on higher number of pieces (12+ pieces). However I expect this OPV to be not as efficient as PPV.

TODO: Average value study for this pruning value

### Layout Pruning Value ###

LPV corresponds to the minimal number of moves needed to restore the block pieces to the block location, regardless of their orientations and permutation. The table size for this is very very low, but I expect it ot be very inefficient

TODO: Average value study for this pruning value


# Search Algorithms #

### Breadth-first search ###

Breadth-first search is the most naive algorithm for searching solution nodes in the search tree. Basically the algorithm applies every possible move to the scrambled state and stores the results as depth-one states. If some of them are solved states, then it retrieves the solutions. If not, then it goes through all depth-one states and applies every possible move to them and stores the depth-two states. It continues with increasing depth values until it finds a solution, or until a given depth limit is reached.

The main advantage of this method is that it theorically finds every solution, starting with the optimal ones. Its major downfall however is the RAM usage, because it needs to store every state at depth d when looking for solutions at depth d+1. The space size at depth d, given a number of possible moves m is O(m^d), making BFS unusable for medium to long solutions.

Pruning values can be used in BFS, but it is not implemented here.

### Depth-first search ###

Depth-first search uses a different strategy than BFS to avoid its memory limitations. Given a depth threshold max_depth, DFS iteratively applies one move to the scrambled state, then to the state that was just obtained, and continues with the children states until it reaches max_depth. This is why it is called depth-first. If no solution was found then DFS steps back to depth max_depth - 1 and applies the depth-first strategy up to max_depth again until there is no more nodes to generate. It then starts again from depth max-depth - 2 and so on until all nodes are visited or solutions have beed found.

DFS does not require as much memory because it only stores the nodes that are on the path to depth d (which is of size O(d)) when looking for solution at depth d+1. Its main downfall is that you need to have an estimate of the optimals length to set the max_depth parameter. If your estimate is too low then no solution will be found, and you will need to relaunch the search with a higher max_depth, and if it is too high then the algorithm will run for longer than necessary and find (possibly many) suboptimal solutions.

DFS can use a pruning value to reduce the search space size. To do so it applies the depth first strategy until it runs into a node at depth d such that d+h = max_depth, where h is the estimated distance of this node to the goal state. If h is an admissible heuristic (e.g. it never overestimates the distance to solved) then DFS will find every solution of length < max_depth. This improvement allows for a faster search.

### Iterative deepening search ###

IDA* needs a heuristic for better performance. It runs a depth-first search at depth h0 where h0 is the estimate for the root node. If no solution is found then IDA* uses the lowest value for max_depth + h that was found during the previous DFS as the new max_depth parameter. It continues with increasing values of max_depth until it reaches the optimal length and solutions are found.