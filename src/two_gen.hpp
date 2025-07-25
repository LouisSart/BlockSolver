#pragma once
#include <cassert>     // assert
#include <filesystem>  // locate table files
#include <fstream>     // write tables into files
#include <map>         // std::map
#include <queue>       // std::deque

#include "223.hpp"  // 2x2x3 solver
#include "coordinate.hpp"
#include "cubie_cube.hpp"
#include "search.hpp"     // IDAstar
#include "step_node.hpp"  // steppers

namespace fs = std::filesystem;
namespace b223 = block_solver_223;

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

bool is_two_gen(const CubieCube& cc) {
    return (cc.cp[DLF] == DLF && cc.co[DLF] == 0) &&  // DLF solved
           (cc.cp[DLB] == DLB && cc.co[DLB] == 0) &&  // DLB solved
           (cc.ep[DF] == DF && cc.eo[DF] == 0) &&     // DF solved
           (cc.ep[DL] == DL && cc.eo[DL] == 0) &&     // DL solved
           (cc.ep[DB] == DB && cc.eo[DB] == 0) &&     // DB solved
           (cc.ep[LF] == LF && cc.eo[LF] == 0) &&     // LF solved
           (cc.ep[LB] == LB && cc.eo[LB] == 0) &&     // LB solved
           (eo_index(cc.eo) == 0);                    // EO solved
}

unsigned corner_index(const CubieCube& cc) {
    static std::array<unsigned, 6> corners{ULF, URF, URB, ULB, DRF, DRB};
    static std::array<unsigned, 6> co;

    assert(is_two_gen(cc));

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

    assert(is_two_gen(cc));

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
PruningTable<N_TWO_GEN_CP * N_TWO_GEN_CO> corner_ptable;
PruningTable<N_TWO_GEN_EP> edge_ptable;
constexpr unsigned NS = b223::NS;

void load_tables() {
    if (corner_ptable.load("two_gen_corners") &&
        edge_ptable.load("two_gen_edges")) {
        return;
    } else {
        std::cout << "generating..." << std::endl;
        corner_ptable.generate(
            CubieCube(),
            [](const Move& move, CubieCube& cc) { cc.apply(move); },
            corner_index, {R, R2, R3, U, U2, U3});
        edge_ptable.generate(
            CubieCube(),
            [](const Move& move, CubieCube& cc) { cc.apply(move); }, edge_index,
            {R, R2, R3, U, U2, U3});
        corner_ptable.write("two_gen_corners");
        edge_ptable.write("two_gen_edges");
    }
}

auto initialize(const Algorithm& scramble) {
    load_tables();
    CubieCube cc;
    cc.apply(scramble);

    return make_root(cc);
}

std::array<unsigned, NS> rotations{
    symmetry_index(0, 3, 0, 0),  // DB
    symmetry_index(0, 0, 0, 0),  // DL
    symmetry_index(0, 1, 0, 0),  // DF
    symmetry_index(0, 2, 0, 0),  // DR
    symmetry_index(0, 3, 1, 0),  // UB
    symmetry_index(0, 0, 1, 0),  // UR
    symmetry_index(0, 2, 1, 0),  // UL
    symmetry_index(0, 1, 1, 0),  // UF
    symmetry_index(2, 0, 0, 0),  // LB
    symmetry_index(2, 1, 0, 0),  // LF
    symmetry_index(2, 3, 0, 0),  // RB
    symmetry_index(2, 2, 0, 0),  // RF
};

auto solve(const Node<CubieCube>::sptr root, const unsigned& max_depth,
           const unsigned& slackness) {
    unsigned sym;
    CubieCube cc = root->state;
    for (unsigned s : rotations) {
        // Find the two_gen_rotation
        if (is_two_gen(cc.get_conjugate(s))) {
            sym = s;
        }
    }

    auto estimate = [&sym](const CubieCube& cc) {
        CubieCube conj = cc.get_conjugate(sym);
        unsigned e_index = edge_ptable[edge_index(conj)];
        unsigned c_index = corner_ptable[corner_index(conj)];
        return std::max(e_index, c_index);
    };

    auto is_solved = [](const CubieCube& cc) { return cc.is_solved(); };
    auto apply = [&sym](const Move& move, CubieCube& cc) { cc.apply(move); };

    auto directions =
        [&sym](const Node<CubieCube>::sptr node) -> std::vector<Move> {
        if (node->parent == nullptr) {
            return {move_anti_conj(R, sym),  move_anti_conj(R2, sym),
                    move_anti_conj(R3, sym), move_anti_conj(U, sym),
                    move_anti_conj(U2, sym), move_anti_conj(U3, sym)};
        } else if (node->last_moves.back() == move_anti_conj(R, sym) ||
                   node->last_moves.back() == move_anti_conj(R2, sym) ||
                   node->last_moves.back() == move_anti_conj(R3, sym)) {
            return {move_anti_conj(U, sym), move_anti_conj(U2, sym),
                    move_anti_conj(U3, sym)};
        } else {
            return {move_anti_conj(R, sym), move_anti_conj(R2, sym),
                    move_anti_conj(R3, sym)};
        }
    };

    auto solutions = IDAstar<false>(root, apply, estimate, is_solved,
                                    directions, max_depth, slackness);
    return solutions;
}

}  // namespace two_gen

namespace two_gen_reduction {

constexpr unsigned NB = 3;
constexpr unsigned NS = b223::NS;
using Cube = std::array<MultiBlockCube<NB>, NS>;
constexpr unsigned N_EQ_CLASSES = 336;   // 336 = 8! / 5!
constexpr unsigned ESIZE = ipow(2, 11);  // Number of possible eo states
constexpr unsigned N_COMB_3EDGES =
    binomial(12, 3);  // Number of states that the 3 LF, DL, LB edges can be
constexpr unsigned TABLE_SIZE = N_EQ_CLASSES * ESIZE * N_COMB_3EDGES;

auto corner_block =
    Block<8, 0>("Corners", {ULF, URF, URB, ULB, DLF, DRF, DRB, DLB}, {});
auto c_m_table = BlockMoveTable(corner_block);
auto eo_m_table = EOMoveTable();
std::array<unsigned, 40320> corner_equivalence_table;
PruningTable<TABLE_SIZE> ptable;

void local_apply(const Move& move, const unsigned& k,
                 MultiBlockCube<NB>& subcube) {
    b223::m_table.sym_apply(move, b223::rotations[k][0], subcube[0]);
    b223::m_table.sym_apply(move, b223::rotations[k][1], subcube[1]);
    c_m_table.sym_apply(move, two_gen::rotations[k], subcube[2]);
    eo_m_table.sym_apply(move, two_gen::rotations[k], subcube[2]);
}

void apply(const Move& move, Cube& cube) {
    for (unsigned k = 0; k < NS; ++k) {
        b223::m_table.sym_apply(move, b223::rotations[k][0], cube[k][0]);
        b223::m_table.sym_apply(move, b223::rotations[k][1], cube[k][1]);
        c_m_table.sym_apply(move, two_gen::rotations[k], cube[k][2]);
        eo_m_table.sym_apply(move, two_gen::rotations[k], cube[k][2]);
    }
};

bool local_is_solved(const MultiBlockCube<NB>& subcube) {
    return (b223::block.is_solved(subcube[0]) &&
            b223::block.is_solved(subcube[1]) &&
            corner_equivalence_table[subcube[2].ccp] == 0 &&
            subcube[2].ceo == 0);
}

auto is_solved = [](const Cube& cube) {
    for (unsigned k = 0; k < NS; ++k) {
        if (local_is_solved(cube[k])) return true;
    }
    return false;
};

auto local_cc_initialize(const CubieCube& scramble_cc, const unsigned k) {
    MultiBlockCube<NB> ret;

    ret[0] = b223::block.to_coordinate_block_cube(
        scramble_cc.get_conjugate(b223::rotations[k][0]));
    ret[1] = b223::block.to_coordinate_block_cube(
        scramble_cc.get_conjugate(b223::rotations[k][1]));
    ret[2] = corner_block.to_coordinate_block_cube(
        scramble_cc.get_conjugate(two_gen::rotations[k]));
    ret[2].ceo =
        eo_index<NE, true>(scramble_cc.get_conjugate(two_gen::rotations[k]).eo);

    return ret;
}

auto cc_initialize(const CubieCube& scramble_cc) {
    Cube ret;

    for (unsigned k = 0; k < NS; ++k) {
        ret[k] = local_cc_initialize(scramble_cc, k);
    }

    return make_root(ret);
}

unsigned phase_2_index(const MultiBlockCube<NB>& cube) {
    unsigned ci = corner_equivalence_table[cube[2].ccp];  // two gen corner
                                                          // equivalence class
    unsigned ei = cube[2].ceo;                            // eo state index
    unsigned cl = cube[0].cel;  // layout coordinate of the 3 edges from DL 123
    return (ci * ESIZE + ei) * N_COMB_3EDGES + cl;
}

unsigned max_estimate(const MultiBlockCube<NB>& cube) {
    unsigned h223 =
        std::max(b223::get_estimate(cube[0]), b223::get_estimate(cube[1]));
    unsigned hphase2 = ptable[phase_2_index(cube)];
    return std::max(h223, hphase2);
}

auto estimate = [](const Cube& cube) {
    unsigned ret = max_estimate(cube[0]);
    for (unsigned k = 0; k < NS; ++k) {
        unsigned e = max_estimate(cube[k]);
        ret = ret < e ? ret : e;
    }
    return ret;
};

void make_corner_equivalence_table() {
    // Reduce the number of corner permutations by using an equivalence index
    // every two permutations with the same equivalence index have the same
    // reduction sequences to two gen.

    two_gen::make_corner_index_table();  // make sure the conversion table is up
    std::array<CubieCube, 120> two_gen_permutations;
    for (unsigned k = 0; k < 120; ++k) {
        unsigned p_index = two_gen::corner_index_table[k];
        two_gen_permutations[k] = CubieCube();
        permutation_from_index(p_index, two_gen_permutations[k].cp);
    }
    unsigned check_counter = 0;

    corner_equivalence_table.fill(N_EQ_CLASSES);
    unsigned class_index = 0;
    std::deque<CubieCube> queue{CubieCube()};
    while (queue.size() > 0) {
        CubieCube cc = queue.back();
        queue.pop_back();

        unsigned index = permutation_index(cc.cp);
        if (corner_equivalence_table[index] == N_EQ_CLASSES) {
            assert(class_index < N_EQ_CLASSES);

            for (const CubieCube& perm : two_gen_permutations) {
                CubieCube eq = perm;
                eq.apply(cc);
                corner_equivalence_table[permutation_index(eq.cp)] =
                    class_index;
            }
            for (auto move : HTM_Moves) {
                CubieCube next = cc;
                next.apply(move);
                queue.push_front(next);
            }
            ++class_index;
        }
    }
}

void load_tables() {
    make_corner_equivalence_table();
    if (ptable.load("two_gen_reduction")) {
        return;
    } else {
        std::cout << "generating..." << std::endl;
        ptable.generate<true>(
            local_cc_initialize(CubieCube(), 1),
            [](const Move& move, MultiBlockCube<NB>& cube) {
                local_apply(move, 1, cube);
            },
            phase_2_index);  // generate the pruning table
        ptable.write("two_gen_reduction");
    }
}

auto initialize(const Algorithm& alg) {
    load_tables();
    CubieCube cc;
    cc.apply(alg);
    return cc_initialize(cc);
}

auto solve(const Node<Cube>::sptr root, const unsigned& max_depth,
           const unsigned& slackness) {
    auto solutions =
        IDAstar<false>(root, apply, estimate, is_solved, max_depth, slackness);
    return solutions;
}

unsigned solved_symmetry(const Cube& cube) {
    unsigned sym = 0;
    for (unsigned k = 0; k < NS; ++k) {
        if (local_is_solved(cube[k])) {
            sym = two_gen::rotations[k];
            return sym;
        }
    }
    assert(false);
    return 0;
}

}  // namespace two_gen_reduction

auto finish =
    make_stepper(make_root<CubieCube>, two_gen::solve, STEPFINAL{}, NONISS);
auto reduction = make_stepper(two_gen_reduction::cc_initialize,
                              two_gen_reduction::solve, finish);