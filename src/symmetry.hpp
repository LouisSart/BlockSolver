#include <cassert>  // make sure arguments are correct
#include <tuple>    // symmetry components

#include "algorithm.hpp"

constexpr unsigned N_SURF = 3;
constexpr unsigned N_Y = 4;
constexpr unsigned N_Z2 = 2;
constexpr unsigned N_LR = 2;
constexpr unsigned N_SYM = 48;

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
// where m' = S * m * S^-1. Using this we can reuse
// the same move and pruning tables for conjugations
// of the original coordinate

// Given a base symmetry S and a move m, these tables
// store the result of m' = S * m * S^-1

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

Move symmetry_move_conj(const Move m, const unsigned index) {
    // Return the
    auto [c_surf, c_y, c_z2, c_lr] = symmetry_index_to_num(index);
    Move ret = m;
    for (unsigned k_lr = 0; k_lr < c_lr; ++k_lr) {
        ret = LR_mirror_move_conj[ret];
    }
    for (unsigned k_z2 = 0; k_z2 < c_z2; ++k_z2) {
        ret = z2_move_conj[ret];
    }
    for (unsigned k_y = 0; k_y < c_y; ++k_y) {
        ret = y_move_conj[ret];
    }
    for (unsigned k_surf = 0; k_surf < c_surf; ++k_surf) {
        ret = S_URF_move_conj[ret];
    }
    return ret;
}

Algorithm symmetry_move_conj(const Algorithm alg, const unsigned index) {
    Algorithm ret = alg;

    for (unsigned k = 0; k < alg.sequence.size(); ++k) {
        ret.sequence[k] = symmetry_move_conj(alg.sequence[k], index);
    }
    return ret;
}