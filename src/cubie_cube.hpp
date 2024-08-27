#pragma once
#include <array>
#include <cstdlib>  // srand, rand
#include <ctime>    // time
#include <iostream>
#include <set>
#include <vector>

#include "algorithm.hpp"   // scramble
#include "coordinate.hpp"  // ipow
#include "cubie.hpp"
#include "move.hpp"      // moves and rotations
#include "symmetry.hpp"  // symmetry components

struct CubieCube {
    Cubie cp[NC]{ULF, URF, URB, ULB, DLF, DRF, DRB, DLB};
    Orientation co[NC]{0, 0, 0, 0, 0, 0, 0, 0};
    Cubie ep[NE]{UF, UR, UB, UL, LF, RF, RB, LB, DF, DR, DB, DL};
    Orientation eo[NE]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    CubieCube(){};
    CubieCube(std::initializer_list<Cubie> cp_in,
              std::initializer_list<Orientation> co_in,
              std::initializer_list<Cubie> ep_in,
              std::initializer_list<Orientation> eo_in) {
        for (Cubie c = ULF; c <= DLB; ++c) {
            cp[c] = *(cp_in.begin() + c);
            co[c] = *(co_in.begin() + c);
        }

        for (Cubie e = UF; e <= DL; e++) {
            ep[e] = *(ep_in.begin() + e);
            eo[e] = *(eo_in.begin() + e);
        }
    }
    CubieCube(const Algorithm& scramble) { apply(scramble); }

    template <typename Contents>
    void print_array(std::string name, int size, const Contents* arr) const {
        std::cout << name << " {";
        for (int i = 0; i < size; i++) {
            std::cout << static_cast<int>(arr[i]);
            if (i < size - 1) {
                std::cout << ",";
            }
        }
        std::cout << "}" << std::endl;
    };

    void show() const {
        // Display the 4 arrays defining a cube at the cubie level
        std::cout << "CubieCube object:" << '\n';
        std::cout << "  ";
        print_array("CP", NC, cp);
        std::cout << "  ";
        print_array("CO", NC, co);
        std::cout << "  ";
        print_array("EP", NE, ep);
        std::cout << "  ";
        print_array("EO", NE, eo);
    };

    // Apply the corner transformation of the given CubieCube
    void corner_apply(const CubieCube& cc) {
        Cubie new_cp[NC];
        Orientation new_co[NC];
        for (Cubie c = ULF; c < NC; c++) {
            new_cp[c] = cp[cc.cp[c]];
            new_co[c] = co[cc.cp[c]] + cc.co[c];
        };
        for (Cubie c = 0; c < NC; c++) {
            cp[c] = new_cp[c];
            co[c] = new_co[c] % 3;
        }
    };

    // Apply the corner transformation of the given CubieCube
    void edge_apply(const CubieCube& cc) {
        Cubie new_ep[NE];
        Orientation new_eo[NE];
        for (Cubie e = UF; e < NE; e++) {
            new_ep[e] = ep[cc.ep[e]];
            new_eo[e] = eo[cc.ep[e]] + cc.eo[e];
        };

        for (Cubie e = UF; e < NE; e++) {
            ep[e] = new_ep[e];
            eo[e] = new_eo[e] % 2;
        }
    };

    // Apply a transformation
    void apply(const CubieCube& cc) {
        edge_apply(cc);
        corner_apply(cc);
    }

    void apply(const CubieCube&& cc) { apply(cc); }

    void apply(const Move&);

    void apply(const Algorithm&);

    // void conjugate(const unsigned sym_index);

    void RL_mirror() {
        // Apply the RL mirroring symmetry. This cannot be
        // applied as a legal cube manipulation

        static CubieCube S_LR{{1, 0, 3, 2, 5, 4, 7, 6},
                              {0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 3, 2, 1, 5, 4, 7, 6, 8, 11, 10, 9},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

        apply(S_LR);
        for (Cubie c = ULF; c < NC; ++c) {
            co[c] = (3 - co[c]) % 3;
        }
        for (Cubie e = UF; e < NE; ++e) {
            eo[e] = (2 - eo[e]) % 2;
        }
    }

    CubieCube get_inverse() const {
        // return the inverse permutation
        CubieCube inverse;
        for (Cubie c = ULF; c < NC; ++c) {
            inverse.cp[cp[c]] = c;
            inverse.co[cp[c]] = (3 - co[c]) % 3;
        };
        for (Cubie e = UF; e < NE; ++e) {
            inverse.ep[ep[e]] = e;
            inverse.eo[ep[e]] = (2 - eo[e]) % 2;
        };
        return inverse;
    }

    void inverse() { *this = get_inverse(); }

    bool operator==(const CubieCube& other) {
        for (Cubie c = ULF; c < NC; ++c) {
            if (cp[c] != other.cp[c]) {
                return false;
            }
            if (co[c] != other.co[c]) {
                return false;
            }
        }
        for (Cubie e = UF; e < NE; ++e) {
            if (ep[e] != other.ep[e]) {
                return false;
            }
            if (eo[e] != other.eo[e]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const CubieCube& other) { return !(*this == other); }

    CubieCube& operator=(const CubieCube& other) {
        if (this == &other)  // self assignment guard
            return *this;

        for (Cubie c = ULF; c < NC; ++c) {
            cp[c] = other.cp[c];
            co[c] = other.co[c];
        }
        for (Cubie e = UF; e < NE; ++e) {
            ep[e] = other.ep[e];
            eo[e] = other.eo[e];
        }
        return *this;
    }

    int corner_parity() const {
        std::set<Cubie> checked;
        std::vector<int> cycle_sizes{0};

        for (Cubie k = ULF; k < NC; ++k) {
            while (checked.find(k) == checked.end()) {
                checked.insert(k);
                ++cycle_sizes.back();
                k = cp[k];
            }
            cycle_sizes.emplace_back(0);
        }

        unsigned number_of_swaps = 0;
        for (auto k : cycle_sizes) {
            if (k != 0) {
                number_of_swaps += (k - 1);
            }
        }
        return ipow(-1, number_of_swaps);
    }

    int edge_parity() const {
        std::set<Cubie> checked;
        std::vector<int> cycle_sizes{0};

        for (Cubie k = UF; k < NE; ++k) {
            while (checked.find(k) == checked.end()) {
                checked.insert(k);
                ++cycle_sizes.back();
                k = ep[k];
            }
            cycle_sizes.emplace_back(0);
        }

        int number_of_swaps = 0;
        for (auto k : cycle_sizes) {
            if (k != 0) {
                number_of_swaps += (k - 1);
            }
        }
        return ipow(-1, number_of_swaps);
    }

    bool has_consistent_co() const {
        int co_sum = 0;
        for (Cubie c = ULF; c < NC; ++c) {
            co_sum += co[c];
        }
        if (co_sum % 3 != 0) {
            return false;
        }
        return true;
    }

    bool has_consistent_eo() const {
        int eo_sum = 0;
        for (Cubie e = UF; e < NE; ++e) {
            eo_sum += eo[e];
        }
        if (eo_sum % 2 != 0) {
            return false;
        }
        return true;
    }

    bool is_solvable() const {
        if (!has_consistent_co()) {
            return false;
        }
        if (!has_consistent_eo()) {
            return false;
        }
        if (corner_parity() != edge_parity()) {
            return false;
        }
        return true;
    }

    bool is_solved() const {
        for (Cubie c = ULF; c < NC; ++c) {
            if (cp[c] != c || co[c] != 0) {
                return false;
            }
        }
        for (Cubie e = UF; e < NE; ++e) {
            if (ep[e] != e || eo[e] != 0) {
                return false;
            }
        }
        return true;
    }
    static CubieCube random_state() {
        CubieCube cube;
        std::srand(std::time(nullptr));

        std::vector<Cubie> corners_left{0, 1, 2, 3, 4, 5, 6, 7};
        for (Cubie c = ULF; c < NC; ++c) {
            auto random = std::rand();
            auto next_idx = random % (NC - c);
            cube.cp[c] = corners_left[next_idx];
            corners_left.erase(corners_left.begin() + next_idx);
        }

        std::vector<Cubie> edges_left{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        for (Cubie e = UF; e < NE; ++e) {
            auto random = std::rand();
            auto next_idx = random % (NE - e);
            cube.ep[e] = edges_left[next_idx];
            edges_left.erase(edges_left.begin() + next_idx);
        }

        int counter = 0;
        for (Cubie c = ULF; c < DLB; ++c) {
            cube.co[c] = std::rand() % 3;
            counter += cube.co[c];
        }
        cube.co[DLB] = (3 - (counter % 3)) % 3;

        counter = 0;
        for (Cubie e = UF; e < DL; ++e) {
            cube.eo[e] = std::rand() % 2;
            counter += cube.eo[e];
        }
        cube.eo[DL] = (2 - (counter % 2)) % 2;

        if (cube.corner_parity() != cube.edge_parity()) {
            auto a = cube.cp[0];
            auto b = cube.cp[1];
            cube.cp[1] = a;
            cube.cp[0] = b;
        }

        return cube;
    }
};

// CubieCube CubieCube::conjugate(const unsigned& sym_index) {
//     apply(elementary_transformations[m]);
// }

// CubieCube rotation_cc[N_ROTATIONS]{

//     [y] = CubieCube{{1, 2, 3, 0, 5, 6, 7, 4},
//                     {0, 0, 0, 0, 0, 0, 0, 0},
//                     {1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8},
//                     {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0}},

//     [z2] = CubieCube{{5, 4, 7, 6, 1, 0, 3, 2},
//                      {0, 0, 0, 0, 0, 0, 0, 0},
//                      {8, 11, 10, 9, 5, 4, 7, 6, 0, 3, 2, 1},
//                      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};

CubieCube move_cc[N_HTM_MOVES]{
    [U] = CubieCube{{1, 2, 3, 0, 4, 5, 6, 7},
                    {0, 0, 0, 0, 0, 0, 0, 0},
                    {1, 2, 3, 0, 4, 5, 6, 7, 8, 9, 10, 11},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [U2] = CubieCube{{2, 3, 0, 1, 4, 5, 6, 7},
                     {0, 0, 0, 0, 0, 0, 0, 0},
                     {2, 3, 0, 1, 4, 5, 6, 7, 8, 9, 10, 11},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [U3] = CubieCube{{3, 0, 1, 2, 4, 5, 6, 7},
                     {0, 0, 0, 0, 0, 0, 0, 0},
                     {3, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [D] = CubieCube{{0, 1, 2, 3, 7, 4, 5, 6},
                    {0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 1, 2, 3, 4, 5, 6, 7, 11, 8, 9, 10},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [D2] = CubieCube{{0, 1, 2, 3, 6, 7, 4, 5},
                     {0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 8, 9},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [D3] = CubieCube{{0, 1, 2, 3, 5, 6, 7, 4},
                     {0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 8},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [R] = CubieCube{{0, 5, 1, 3, 4, 6, 2, 7},
                    {0, 2, 1, 0, 0, 1, 2, 0},
                    {0, 5, 2, 3, 4, 9, 1, 7, 8, 6, 10, 11},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [R2] = CubieCube{{0, 6, 5, 3, 4, 2, 1, 7},
                     {0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 9, 2, 3, 4, 6, 5, 7, 8, 1, 10, 11},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [R3] = CubieCube{{0, 2, 6, 3, 4, 1, 5, 7},
                     {0, 2, 1, 0, 0, 1, 2, 0},
                     {0, 6, 2, 3, 4, 1, 9, 7, 8, 5, 10, 11},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [L] = CubieCube{{3, 1, 2, 7, 0, 5, 6, 4},
                    {1, 0, 0, 2, 2, 0, 0, 1},
                    {0, 1, 2, 7, 3, 5, 6, 11, 8, 9, 10, 4},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [L2] = CubieCube{{7, 1, 2, 4, 3, 5, 6, 0},
                     {0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 1, 2, 11, 7, 5, 6, 4, 8, 9, 10, 3},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [L3] = CubieCube{{4, 1, 2, 0, 7, 5, 6, 3},
                     {1, 0, 0, 2, 2, 0, 0, 1},
                     {0, 1, 2, 4, 11, 5, 6, 3, 8, 9, 10, 7},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [F] = CubieCube{{4, 0, 2, 3, 5, 1, 6, 7},
                    {2, 1, 0, 0, 1, 2, 0, 0},
                    {4, 1, 2, 3, 8, 0, 6, 7, 5, 9, 10, 11},
                    {1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0}},

    [F2] = CubieCube{{5, 4, 2, 3, 1, 0, 6, 7},
                     {0, 0, 0, 0, 0, 0, 0, 0},
                     {8, 1, 2, 3, 5, 4, 6, 7, 0, 9, 10, 11},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [F3] = CubieCube{{1, 5, 2, 3, 0, 4, 6, 7},
                     {2, 1, 0, 0, 1, 2, 0, 0},
                     {5, 1, 2, 3, 0, 8, 6, 7, 4, 9, 10, 11},
                     {1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0}},

    [B] = CubieCube{{0, 1, 6, 2, 4, 5, 7, 3},
                    {0, 0, 2, 1, 0, 0, 1, 2},
                    {0, 1, 6, 3, 4, 5, 10, 2, 8, 9, 7, 11},
                    {0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0}},

    [B2] = CubieCube{{0, 1, 7, 6, 4, 5, 3, 2},
                     {0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 1, 10, 3, 4, 5, 7, 6, 8, 9, 2, 11},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    [B3] = CubieCube{{0, 1, 3, 7, 4, 5, 2, 6},
                     {0, 0, 2, 1, 0, 0, 1, 2},
                     {0, 1, 7, 3, 4, 5, 2, 10, 8, 9, 6, 11},
                     {0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0}}};

void CubieCube::apply(const Move& m) { apply(move_cc[m]); }

void CubieCube::apply(const Algorithm& alg) {
    for (const Move& m : alg.sequence) {
        apply(m);
    }
}