# EpicSolver

This program can solve the Rubik's Cube using various methods. 


clone with :

```shell
$ git clone --recursive https://github.com/LouisSart/BlockSolver.git
```

compile with :
```shell
$ make epic
```

run with (example) :
```shell
$ ./obj/epic/l3c <scramble>
```

scramble should be written in Singmaster notation with one blank space as the separator.

## Commands

`l3c`: solve a 2x2x2, extend it to an F2L-1 block and finish by leaving a 3c skeleton

## Options

`-M`: set the maximum depth of search (default 30)

## TODO

Generate rotations and inverse of starting position
Retrieve skeleton
Allow using with a move that cancels with previous step
Tune solver parameters