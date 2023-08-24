# BlockSolver
A Rubik's Cube block solver

### CubieCube ###
A struct defining 4 arrays for cp, co, ep and eo

### BlockCube ###
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

### CoordinateBlockCube ###
The coordinate representation of a subgroup of the cube pieces
(for example the three edges and one corner of a 2x2x2 block)
There are six coordinates that represent the state of the cubies:

ccl: the positions of the corners regardless of their order
ccp: the permutation of the corners
cco: the orientation of the corners
cel: the positions of the edges regardless of their order
cep: the permutation of the edges
ceo: the orientation of the edges
