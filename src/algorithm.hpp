#pragma once
#include <array>
#include <iostream>
#include <vector>

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
    S_URF2,
    NoneMove = -1
};

struct Algorithm {
    std::vector<Move> sequence;

    Algorithm(){};
    Algorithm(const std::vector<Move>& s) : sequence{s} {};

    void append(const Algorithm& other) {
        sequence.insert(sequence.end(), other.sequence.begin(),
                        other.sequence.end());
    }

    void append(const Move& move) { sequence.push_back(move); }

    Move back() {
        if (sequence.size() == 0) {
            return NoneMove;
        }
        return sequence.back();
    }

    auto size() const {
        unsigned my_size = 0;
        for (Move move : sequence) {
            if (move < x && move >= 0) {
                ++my_size;
            }
        }
        return my_size;
    }

    void show() const;
};

constexpr unsigned N_HTM_MOVES = 18;
constexpr unsigned N_HTM_MOVES_AND_ROTATIONS = 27;
constexpr unsigned N_HTM_MOVES_AND_SYMMETRIES = 29;

std::array<std::string, N_HTM_MOVES_AND_SYMMETRIES> move_str{
    "U",  "U2", "U'", "D",  "D2", "D'", "R",  "R2",  "R'",   "L",
    "L2", "L'", "F",  "F2", "F'", "B",  "B2", "B'",  "x",    "x2",
    "x'", "y",  "y2", "y'", "z",  "z2", "z'", "x y", "x' z'"};

void Algorithm::show() const {
    for (auto&& m : sequence) {
        if (m != NoneMove) {
            std::cout << move_str[m] << " ";
        }
    }
    std::cout << "(" << size() << ")" << std::endl;
}

std::array<Move, N_HTM_MOVES> HTM_Moves{U, U2, U3, D, D2, D3, R, R2, R3,
                                        L, L2, L3, F, F2, F3, B, B2, B3};
std::array<Move, N_HTM_MOVES_AND_ROTATIONS> HTM_Moves_and_rotations{
    U,  U2, U3, D,  D2, D3, R,  R2, R3, L,  L2, L3, F,
    F2, F3, B,  B2, B3, x,  x2, x3, y,  y2, y3, z,  z2};
std::vector<Move> after_U{D,  D2, D3, R,  R2, R3, L, L2,
                          L3, F,  F2, F3, B,  B2, B3};
std::vector<Move> after_D{R, R2, R3, L, L2, L3, F, F2, F3, B, B2, B3};
std::vector<Move> after_R{U,  U2, U3, D,  D2, D3, L, L2,
                          L3, F,  F2, F3, B,  B2, B3};
std::vector<Move> after_L{U, U2, U3, D, D2, D3, F, F2, F3, B, B2, B3};
std::vector<Move> after_F{U,  U2, U3, D,  D2, D3, R, R2,
                          R3, L,  L2, L3, B,  B2, B3};
std::vector<Move> after_B{U, U2, U3, D, D2, D3, R, R2, R3, L, L2, L3};
std::vector<Move> after_None{U, U2, U3, D, D2, D3, R, R2, R3,
                             L, L2, L3, F, F2, F3, B, B2, B3};

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
            return after_None;
    }
}