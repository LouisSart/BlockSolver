#pragma once
#include <cassert>     // assert
#include <filesystem>  // locate table files
#include <fstream>     // write tables into files
#include <map>         // std::map
#include <queue>       // std::deque

#include "223.hpp"  // 2x2x3 solver
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
    corner_index_table.fill(40320);
    while (!queue.empty()) {
        CubieCube cc = queue.back();
        queue.pop_back();
        unsigned index = pairing_index(cc);
        if (corner_index_table[index] == 40320) {
            corner_index_table[index] = permutation_index(cc.cp);

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

namespace two_gen_reduction {
namespace b223 = block_solver_223;

constexpr unsigned CSIZE = factorial(8);
constexpr unsigned ESIZE = ipow(2, 11);
constexpr unsigned NB = 3;
constexpr unsigned NS = b223::NS;
using Cube = std::array<MultiBlockCube<NB>, NS>;

unsigned cp_eo_index(const CubieCube& cc) {
    unsigned cpi = permutation_index<8>(cc.cp);
    unsigned eoi = eo_index<12, true>(cc.eo);
    assert(cpi < CSIZE);
    assert(eoi < ESIZE);

    return cpi * ESIZE + eoi;
}

auto corner_block =
    Block<8, 0>("Corners", {ULF, URF, URB, ULB, DLF, DRF, DRB, DLB}, {});
auto c_m_table = BlockMoveTable(corner_block);
auto eo_m_table = EOMoveTable();

void local_apply(const Move& move, const std::array<unsigned, b223::NB>& syms,
                 MultiBlockCube<NB>& subcube) {
    b223::m_table.sym_apply(move, syms[0], subcube[0]);
    b223::m_table.sym_apply(move, syms[1], subcube[1]);
    c_m_table.sym_apply(move, syms[0], subcube[2]);
    eo_m_table.sym_apply(move, syms[0], subcube[2]);
}

void apply(const Move& move, Cube& cube) {
    for (unsigned k = 0; k < NS; ++k) {
        local_apply(move, b223::rotations[k], cube[k]);
    }
};

bool local_is_solved(const MultiBlockCube<NB>& subcube) {
    return (b223::block.is_solved(subcube[0]) &&
            b223::block.is_solved(subcube[1]) &&
            corner_block.is_solved(subcube[2]) && subcube[2].ceo == 0);
}

auto is_solved = [](const Cube& cube) {
    for (unsigned k = 0; k < NS; ++k) {
        if (local_is_solved(cube[k])) return true;
    }
    return false;
};

auto cc_initialize(const CubieCube& scramble_cc) {
    Cube ret;

    for (unsigned k = 0; k < NS; ++k) {
        ret[k][0] = b223::block.to_coordinate_block_cube(
            scramble_cc.get_conjugate(b223::rotations[k][0]));
        ret[k][1] = b223::block.to_coordinate_block_cube(
            scramble_cc.get_conjugate(b223::rotations[k][1]));
        ret[k][2] = corner_block.to_coordinate_block_cube(
            scramble_cc.get_conjugate(b223::rotations[k][0]));
        ret[k][2].ceo = eo_index<NE, true>(scramble_cc.eo);
    }

    return make_root(ret);
}

// CHECKME -> marche en théorie mais prend un temps fou
constexpr unsigned TABLE_SIZE = CSIZE * ESIZE;
std::array<unsigned, TABLE_SIZE> cp_eo_table;
void make_pruning_table() {
    cp_eo_table.fill(255);   // initialize the table with 255
    two_gen::load_tables();  // make sure the two_gen table is loaded

    unsigned set_count = 0;
    for (unsigned k : two_gen::corner_index_table) {
        cp_eo_table[k * ESIZE] = 0;  // every 2gen position set to h=0
        set_count++;
    }

    CoordinateBlockCube cbc;
    unsigned depth = 0;
    while (set_count < TABLE_SIZE) {
        for (unsigned k = 0; k < TABLE_SIZE; ++k) {
            if (cp_eo_table[k] == 255) {
                unsigned cpi = k / ESIZE;
                unsigned eoi = k % ESIZE;
                cbc.set(0, 0, cpi, 0, 0, eoi);

                for (auto move : default_directions) {
                    auto child = cbc;
                    c_m_table.apply(move, child);
                    eo_m_table.apply(move, child);

                    auto child_index = child.ccp * ESIZE + child.ceo;

                    if (cp_eo_table[child_index] == depth) {
                        cp_eo_table[k] = depth + 1;
                        set_count++;
                        break;  // found a child, no need to check others
                    }
                }
            }
        }
        ++depth;
    }

    for (auto k : cp_eo_table) {
        assert(k < 255);  // all entries should be less than 255
    }
}

}  // namespace two_gen_reduction