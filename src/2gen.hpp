#pragma once
#include <queue>

#include "coordinate.hpp"
#include "cubie_cube.hpp"

using Pair = std::set<Cubie>;

void show(const Pair& pair) {
    std::cout << "{";
    for (const unsigned c : pair) {
        std::cout << (Corner)c << ",";
    }
    std::cout << "\b}";
}

using Pairing = std::set<Pair>;

void show(const Pairing& pairing) {
    for (const Pair& p : pairing) {
        show(p);
    }
    std::cout << std::endl;
}

std::array<Pairing, 5> pairings{
    Pairing{{ULB, URB}, {ULF, DRB}, {URF, DRF}},  // V
    Pairing{{ULB, DRB}, {ULF, DRF}, {URF, URB}},  // W
    Pairing{{ULF, ULB}, {URF, DRB}, {URB, DRF}},  // X
    Pairing{{ULF, URB}, {URF, ULB}, {DRF, DRB}},  // Y
    Pairing{{ULF, URF}, {ULB, DRF}, {URB, DRB}}   // Z
};

Pair get_pair(const CubieCube& cc, const Pair& p) {
    Pair ret;
    for (const Cubie& c : p) {
        assert(cc.cp[c] != NONE_C);
        assert(cc.cp[c] != DLF);
        assert(cc.cp[c] != DLB);
        ret.insert(cc.cp[c]);
    }
    return ret;
}

Pairing get_pairing(const CubieCube& cc, const Pairing& pairing) {
    Pairing ret;
    for (const Pair pair : pairing) {
        ret.insert(get_pair(cc, pair));
    }
    return ret;
}

unsigned pairing_to_index(const std::set<Pair>& pairing) {
    for (unsigned i = 0; i < pairings.size(); ++i) {
        if (pairings[i] == pairing) {
            return i;
        }
    }
    assert(false);  // pairing not found, cube not two gen
    return 0;
}

unsigned pairing_index(const CubieCube& cc) {
    std::array<unsigned, 5> perm;
    for (unsigned i = 0; i < 5; ++i) {
        perm[i] = pairing_to_index(get_pairing(cc, pairings[i]));
    }
    return permutation_index(perm);
}

unsigned two_gen_corner_index(const CubieCube& cc) {
    static std::array<unsigned, 6> corners{ULF, URF, URB, ULB, DRF, DRB};
    static std::array<unsigned, 6> co;

    assert(cc.cp[DLF] == DLF && cc.co[DLF] == 0);  // DLF solved
    assert(cc.cp[DLB] == DLB && cc.co[DLB] == 0);  // DLB solved

    for (unsigned k = 0; k < 6; ++k) {
        co[k] = cc.co[corners[k]];
    }

    unsigned cco = co_index<6, true>(co);
    unsigned ccp = pairing_index(cc);

    return cco * 120 + ccp;
}

unsigned two_gen_edge_index(const CubieCube& cc) {
    static std::array<unsigned, 7> edges{UF, UR, UB, UL, RF, RB, DR};
    static std::array<unsigned, 7> ep;

    assert(cc.ep[DF] == DF && cc.eo[DF] == 0);  // DF solved
    assert(cc.ep[DL] == DL && cc.eo[DL] == 0);  // DL solved
    assert(cc.ep[DB] == DB && cc.eo[DB] == 0);  // DB solved
    assert(cc.ep[LF] == LF && cc.eo[LF] == 0);  // LF solved
    assert(cc.ep[LB] == LB && cc.eo[LB] == 0);  // LB solved
    assert(eo_index(cc.eo) == 0);               // EO solved

    for (unsigned k = 0; k < 7; ++k) {
        ep[k] = cc.ep[edges[k]];
    };

    return permutation_index(ep);
}

unsigned two_gen_index(const CubieCube& cc) {
    return two_gen_corner_index(cc) * 5040 + two_gen_edge_index(cc);
}

std::array<unsigned, 120> two_gen_corner_index_table;

void make_corner_index_table() {
    std::deque<CubieCube> queue;
    queue.push_back(CubieCube());
    two_gen_corner_index_table.fill(40321);
    while (!queue.empty()) {
        CubieCube cc = queue.back();
        queue.pop_back();
        unsigned index = pairing_index(cc);
        if (two_gen_corner_index_table[index] == 40321) {
            std::array<unsigned, 8> cp{cc.cp[0], cc.cp[1], cc.cp[2], cc.cp[3],
                                       cc.cp[4], cc.cp[5], cc.cp[6], cc.cp[7]};
            two_gen_corner_index_table[index] = permutation_index(cp);

            for (const Move m : {R, U}) {
                CubieCube next = cc;
                next.apply(m);
                queue.push_back(next);
            }
        }
    }
};

// std::array<unsigned, 5040 * 6> two_gen_edge_mtable;
// std::array<unsigned, 120 * 243 * 6> two_gen_corner_mtable;
// struct TwoGenCube {
//     unsigned corner_index;
//     unsigned edge_index;
// };

std::array<unsigned, 5040 * 29160 / 2> two_gen_table;
void make_two_gen_pruning_table() {
    std::fill(two_gen_table.begin(), two_gen_table.end(), 255);

    std::queue<CubieCube> queue;
    queue.push(CubieCube());

    two_gen_table[two_gen_index(CubieCube())] = 0;

    while (!queue.empty()) {
        CubieCube cc = queue.front();
        queue.pop();

        unsigned index = two_gen_index(cc);
        unsigned depth = two_gen_table[index];

        std::array<Move, 6> two_gen_moves{U, U2, U, R, R2, R};
        for (unsigned k = 0; k < 6; ++k) {
            CubieCube cc2 = cc;
            cc2.apply(two_gen_moves[k]);

            unsigned index2 = two_gen_index(cc2);
            if (two_gen_table[index2] == 255) {
                two_gen_table[index2] = depth + 1;
                queue.push(cc2);
            }
        }
    }
}