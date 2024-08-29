#pragma once
#include <cassert>  // make sure arguments are correct
#include <tuple>    // symmetry components

#include "algorithm.hpp"

constexpr unsigned N_SURF = 3;
constexpr unsigned N_Y = 4;
constexpr unsigned N_Z2 = 2;
constexpr unsigned N_LR = 2;
constexpr unsigned N_SYM = 48;
constexpr unsigned N_ELEM_SYM = 4;

unsigned symmetry_index(const unsigned c_surf, const unsigned c_y,
                        const unsigned c_z2, const unsigned c_lr) {
    // Assign a unique index to each combination of symmetries

    assert(c_surf < N_SURF);
    assert(c_y < N_Y);
    assert(c_z2 < N_Z2);
    assert(c_lr < N_LR);

    unsigned coord = c_lr + N_LR * (c_z2 + N_Z2 * (c_y + N_Y * (c_surf)));

    assert(coord < N_SYM);
    return coord;
}

auto symmetry_index_to_num(const unsigned index) {
    // Retrieve the symmetry components from the given index

    assert(index < N_SYM);

    unsigned div = index;
    unsigned c_lr = div % N_LR;
    div = div / N_LR;
    unsigned c_z2 = div % N_Z2;
    div = div / N_Z2;
    unsigned c_y = div % N_Y;
    div = div / N_Y;
    unsigned c_surf = div;

    assert(c_surf < N_SURF);
    assert(c_y < N_Y);
    assert(c_z2 < N_Z2);
    assert(c_lr < N_LR);

    return std::make_tuple(c_surf, c_y, c_z2, c_lr);
}

// Let c be a given permutation of the cube
// Given a symmetry S, let conj be the function:

// conj(c, S) = S^-1 * c * S

// If we were to work on a coordinate level and we wanted
// to compute the coordinate for:

// conj(c, S) * m, where m is a legal move

// we would face an issue because the move table
// can only transform the coordinate that represents c
// but not its conjugation. However:
// conj(c, S) * m = S^-1 * c * S * m
//                = S^-1 * c * S * S^-1 * S * m * S^-1 * S
//                = S^-1 * (c * S * m * S^-1) * S
//                = conj(c * m', S)
// where m' = S * m * S^-1.
// This file is where the translation of moves after conjugation
// by a symmetry is computed

Move S_URF_move_conj[N_HTM_MOVES] = {
    [U] = F, [U2] = F2, [U3] = F3, [D] = B, [D2] = B2, [D3] = B3,
    [R] = U, [R2] = U2, [R3] = U3, [L] = D, [L2] = D2, [L3] = D3,
    [F] = R, [F2] = R2, [F3] = R3, [B] = L, [B2] = L2, [B3] = L3};

Move y_move_conj[N_HTM_MOVES] = {
    [U] = U, [U2] = U2, [U3] = U3, [D] = D, [D2] = D2, [D3] = D3,
    [R] = B, [R2] = B2, [R3] = B3, [L] = F, [L2] = F2, [L3] = F3,
    [F] = R, [F2] = R2, [F3] = R3, [B] = L, [B2] = L2, [B3] = L3};

Move z2_move_conj[N_HTM_MOVES] = {
    [U] = D, [U2] = D2, [U3] = D3, [D] = U, [D2] = U2, [D3] = U3,
    [R] = L, [R2] = L2, [R3] = L3, [L] = R, [L2] = R2, [L3] = R3,
    [F] = F, [F2] = F2, [F3] = F3, [B] = B, [B2] = B2, [B3] = B3};

Move LR_mirror_move_conj[N_HTM_MOVES] = {
    [U] = U3, [U2] = U2, [U3] = U, [D] = D3, [D2] = D2, [D3] = D,
    [R] = L3, [R2] = L2, [R3] = L, [L] = R3, [L2] = R2, [L3] = R,
    [F] = F3, [F2] = F2, [F3] = F, [B] = B3, [B2] = B2, [B3] = B};

void permute_moves(Move* mp1, const Move* mp2) {
    // Perform mp1 o mp2

    Move new_mp[N_HTM_MOVES];
    for (Move m : HTM_Moves) {
        new_mp[m] = mp1[mp2[m]];
    };

    for (Move m : HTM_Moves) {
        mp1[m] = new_mp[m];
    }
}

Move* get_move_permutation(const unsigned& sym_index) {
    Move* ret = new Move[N_HTM_MOVES]{U, U2, U3, D, D2, D3, R, R2, R3,
                                      L, L2, L3, F, F2, F3, B, B2, B3};
    auto [c_surf, c_y, c_z2, c_lr] = symmetry_index_to_num(sym_index);

    for (unsigned k_lr = 0; k_lr < c_lr; ++k_lr) {
        permute_moves(ret, LR_mirror_move_conj);
    }
    for (unsigned k_z2 = 0; k_z2 < c_z2; ++k_z2) {
        permute_moves(ret, z2_move_conj);
    }
    for (unsigned k_y = 0; k_y < c_y; ++k_y) {
        permute_moves(ret, y_move_conj);
    }
    for (unsigned k_surf = 0; k_surf < c_surf; ++k_surf) {
        permute_moves(ret, S_URF_move_conj);
    }

    return ret;
}