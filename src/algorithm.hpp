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
    S_URF,
    S_URF2,
    S_z2,
    S_y,
    S_y2,
    S_y3,
    S_LR,
    NoneMove = -1
};

struct Algorithm {
    std::vector<Move> sequence;

    Algorithm(){};
    Algorithm(std::vector<Move> s) : sequence{s} {};

    void show();
};

std::array<std::string, 18> move_str{"U", "U2", "U'", "D", "D2", "D'",
                                     "R", "R2", "R'", "L", "L2", "L'",
                                     "F", "F2", "F'", "B", "B2", "B'"};
void Algorithm::show() {
    int size = 0;
    for (auto&& m : sequence) {
        if (m != NoneMove) {
            std::cout << move_str[m] << " ";
            size += 1;
        }
    }
    std::cout << "(" << size << ")" << std::endl;
}
// 0:U 1:U2 2:U' 3:D 4:D2 5:D' 6:R 7:R2 8:R' 9:L 10:L2 11:L' 12:F 13:F2 14:F'
// 15:B 16:B2 17:B' 18:S_URF 19:S_URF2 20:z2 21:y 22:y2 23:y' 24:S_LR
std::array<Move, 18> HTM_Moves{U, U2, U3, D, D2, D3, R, R2, R3,
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
std::vector<Move> after_None{U, U2, U3, D, D2, D3, R, R2, R3,
                             L, L2, L3, F, F2, F3, B, B2, B3};

std::vector<Move>& allowed_next(const Move m) {
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