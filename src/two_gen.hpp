#pragma once
#include <cassert>     // assert
#include <filesystem>  // locate table files
#include <fstream>     // write tables into files
#include <queue>       // std::deque

#include "coordinate.hpp"
#include "cubie_cube.hpp"
#include "search.hpp"  // IDAstar

namespace fs = std::filesystem;

namespace two_gen {
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

unsigned corner_index(const CubieCube& cc) {
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

unsigned edge_index(const CubieCube& cc) {
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

unsigned index(const CubieCube& cc) {
    return (corner_index(cc) * 5040 + edge_index(cc)) / 2;
}

std::array<unsigned, 120> corner_index_table;
void make_corner_index_table() {
    std::deque<CubieCube> queue;
    queue.push_back(CubieCube());
    corner_index_table.fill(40321);
    while (!queue.empty()) {
        CubieCube cc = queue.back();
        queue.pop_back();
        unsigned index = pairing_index(cc);
        if (corner_index_table[index] == 40321) {
            std::array<unsigned, 8> cp{cc.cp[0], cc.cp[1], cc.cp[2], cc.cp[3],
                                       cc.cp[4], cc.cp[5], cc.cp[6], cc.cp[7]};
            corner_index_table[index] = permutation_index(cp);

            for (const Move m : {R, U}) {
                CubieCube next = cc;
                next.apply(m);
                queue.push_back(next);
            }
        }
    }
};

std::array<Move, 6> moves{U, U2, U3, R, R2, R3};
constexpr unsigned N_TWO_GEN_CP = factorial(5);
constexpr unsigned N_TWO_GEN_CO = ipow(3, 5);
constexpr unsigned N_TWO_GEN_EP = factorial(7);
std::array<unsigned, N_TWO_GEN_CP * N_TWO_GEN_CO> corner_ptable;
std::array<unsigned, N_TWO_GEN_EP> edge_ptable;

template <std::size_t N, typename Indexer, std::size_t NM>
void make_pruning_table(std::array<unsigned, N>& ptable, const Indexer& index,
                        std::array<Move, NM>& moves) {
    std::fill(ptable.begin(), ptable.end(), 255);

    std::deque<CubieCube> queue{CubieCube()};

    ptable[index(CubieCube())] = 0;

    while (!queue.empty()) {
        CubieCube cc = queue.back();

        unsigned i = index(cc);
        unsigned depth = ptable[i];
        assert(i < ptable.size());

        for (unsigned k = 0; k < 6; ++k) {
            CubieCube cc2 = cc;
            cc2.apply(moves[k]);

            unsigned ii = index(cc2);
            assert(ii < ptable.size());
            if (ptable[ii] == 255) {
                ptable[ii] = depth + 1;
                queue.push_front(cc2);
            }
        }
        queue.pop_back();
    }
    for (unsigned k = 0; k < 5040; ++k) {
        assert(edge_ptable[k] < 255);
    }
}

auto filedir = fs::current_path() / "pruning_tables" / "two_gen";
auto corner_filepath = filedir / "corners";
auto edge_filepath = filedir / "edges";

void write_tables() {
    fs::create_directories(filedir);
    {
        std::ofstream file(corner_filepath, std::ios::binary);
        file.write(reinterpret_cast<char*>(corner_ptable.data()),
                   sizeof(unsigned) * corner_ptable.size());
        file.close();
    }
    {
        std::ofstream file(edge_filepath, std::ios::binary);
        file.write(reinterpret_cast<char*>(edge_ptable.data()),
                   sizeof(unsigned) * edge_ptable.size());
        file.close();
    }
}

void load_tables() {
    if (fs::exists(corner_filepath) && fs::exists(edge_filepath)) {
        assert(fs::exists(corner_filepath));
        {
            std::ifstream file(corner_filepath, std::ios::binary);
            file.read(reinterpret_cast<char*>(corner_ptable.data()),
                      sizeof(unsigned) * corner_ptable.size());
            file.close();
        }
        assert(fs::exists(edge_filepath));
        {
            std::ifstream file(edge_filepath, std::ios::binary);
            file.read(reinterpret_cast<char*>(edge_ptable.data()),
                      sizeof(unsigned) * edge_ptable.size());
            file.close();
        }
    } else {
        std::cout << "Pruning table not found, generating" << std::endl;
        make_pruning_table(corner_ptable, corner_index, moves);
        make_pruning_table(edge_ptable, edge_index, moves);
        write_tables();
    }
}

unsigned estimate(const CubieCube& cc) {
    unsigned e_index = edge_ptable[edge_index(cc)];
    unsigned c_index = corner_ptable[corner_index(cc)];
    return std::max(e_index, c_index);
}

auto initialize(const Algorithm& scramble) {
    load_tables();
    CubieCube cc;
    cc.apply(scramble);

    return make_root(cc);
}

bool is_solved(const CubieCube& cc) { return cc.is_solved(); }
void apply(const Move& move, CubieCube& cc) { cc.apply(move); }

std::vector<Move> directions(const Node<CubieCube>::sptr node) {
    if (node->parent == nullptr) {
        return {R, R2, R3, U, U2, U3};
    } else if (node->last_moves.back() == R || node->last_moves.back() == R2 ||
               node->last_moves.back() == R3) {
        return {U, U2, U3};
    } else {
        return {R, R2, R3};
    }
}

auto solve(const Node<CubieCube>::sptr root, const unsigned& max_depth,
           const unsigned& slackness) {
    auto solutions = IDAstar<false>(root, apply, estimate, is_solved,
                                    directions, max_depth, slackness);
    return solutions;
}

}  // namespace two_gen