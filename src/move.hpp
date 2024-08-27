#pragma once
#include <array>          // arrays of move strings
#include <iostream>       // std::cout << Move
#include <sstream>        // std::string
#include <unordered_map>  // map move to str

enum Move {
    U,
    U2,
    U3,
    D,
    D2,
    D3,
    R,
    R2,
    R3,
    L,
    L2,
    L3,
    F,
    F2,
    F3,
    B,
    B2,
    B3,
    x,
    x2,
    x3,
    y,
    y2,
    y3,
    z,
    z2,
    z3,
    S_URF,
    S_URF2
};

std::unordered_map<std::string, Move> str_to_move = {
    {"U", U}, {"U2", U2}, {"U'", U3}, {"D", D}, {"D2", D2}, {"D'", D3},
    {"R", R}, {"R2", R2}, {"R'", R3}, {"L", L}, {"L2", L2}, {"L'", L3},
    {"F", F}, {"F2", F2}, {"F'", F3}, {"B", B}, {"B2", B2}, {"B'", B3}};

constexpr unsigned N_HTM_MOVES = 18;
constexpr unsigned N_HTM_MOVES_AND_ROTATIONS = 27;
constexpr unsigned N_HTM_MOVES_AND_SYMMETRIES = 29;

std::array<std::string, N_HTM_MOVES_AND_SYMMETRIES> move_str{
    "U",  "U2", "U'", "D",  "D2", "D'", "R",  "R2",  "R'",   "L",
    "L2", "L'", "F",  "F2", "F'", "B",  "B2", "B'",  "x",    "x2",
    "x'", "y",  "y2", "y'", "z",  "z2", "z'", "x y", "x' z'"};

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << move_str[move];
    return os;
}

std::array<Move, N_HTM_MOVES> HTM_Moves{U, U2, U3, D, D2, D3, R, R2, R3,
                                        L, L2, L3, F, F2, F3, B, B2, B3};
std::array<Move, N_HTM_MOVES_AND_ROTATIONS> HTM_Moves_and_rotations{
    U,  U2, U3, D,  D2, D3, R,  R2, R3, L,  L2, L3, F, F2,
    F3, B,  B2, B3, x,  x2, x3, y,  y2, y3, z,  z2, z3};
std::array<Move, N_HTM_MOVES_AND_ROTATIONS> inverse_of_HTM_Moves_and_rotations{
    U3, U2, U,  D3, D2, D,  R3, R2, R,  L3, L2, L,  F3, F2,
    F,  B3, B2, B,  x3, x2, x,  y3, y2, y,  z3, z2, z};

std::vector<Move> default_directions{U, U2, U3, D, D2, D3, R, R2, R3,
                                     L, L2, L3, F, F2, F3, B, B2, B3};
std::vector<Move> after_U{D,  D2, D3, R,  R2, R3, L, L2,
                          L3, F,  F2, F3, B,  B2, B3};
std::vector<Move> after_D{R, R2, R3, L, L2, L3, F, F2, F3, B, B2, B3};
std::vector<Move> after_R{U,  U2, U3, D,  D2, D3, L, L2,
                          L3, F,  F2, F3, B,  B2, B3};
std::vector<Move> after_L{U, U2, U3, D, D2, D3, F, F2, F3, B, B2, B3};
std::vector<Move> after_F{U,  U2, U3, D,  D2, D3, R, R2,
                          R3, L,  L2, L3, B,  B2, B3};
std::vector<Move> after_B{U, U2, U3, D, D2, D3, R, R2, R3, L, L2, L3};

const std::vector<Move>& allowed_next(const Move m) {
    // Utility function that takes in the last move applied to the current node
    // and returns the set of all moves of different faces
    switch (m) {
        case U ... U3:
            return after_U;
        case D ... D3:
            return after_D;
        case R ... R3:
            return after_R;
        case L ... L3:
            return after_L;
        case F ... F3:
            return after_F;
        case B ... B3:
            return after_B;
        default:
            return default_directions;
    }
}