#pragma once
#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <vector>

#include "algorithm.hpp"
#include "coordinate.hpp"

struct CubieCube {
    uint cp[8]{0, 1, 2, 3, 4, 5, 6, 7};
    uint co[8]{0, 0, 0, 0, 0, 0, 0, 0};
    uint ep[12]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    uint eo[12]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    CubieCube(){};
    CubieCube(std::initializer_list<uint> cp_in,
              std::initializer_list<uint> co_in,
              std::initializer_list<uint> ep_in,
              std::initializer_list<uint> eo_in) {
        for (int i = 0; i < 8; i++) {
            cp[i] = *(cp_in.begin() + i);
            co[i] = *(co_in.begin() + i);
        }

        for (int i = 0; i < 12; i++) {
            ep[i] = *(ep_in.begin() + i);
            eo[i] = *(eo_in.begin() + i);
        }
    }

    void show() const {
        auto print_array = [](std::string name, int size, const uint* arr) {
            std::cout << name << " {";
            for (int i = 0; i < size; i++) {
                std::cout << arr[i];
                if (i < size - 1) {
                    std::cout << ",";
                }
            }
            std::cout << "}" << std::endl;
        };

        // Display the 4 arrays defining a cube at the cubie level
        std::cout << "CubieCube object:" << '\n';
        std::cout << "  ";
        print_array("CP", 8, cp);
        std::cout << "  ";
        print_array("CO", 8, co);
        std::cout << "  ";
        print_array("EP", 12, ep);
        std::cout << "  ";
        print_array("EO", 12, eo);
    };

    // Apply the corner transformation of the given CubieCube
    void corner_apply(CubieCube& cc) {
        int new_cp[8], new_co[8];
        for (int i = 0; i < 8; i++) {
            new_cp[i] = cp[cc.cp[i]];
            new_co[i] = co[cc.cp[i]] + cc.co[i];
        };
        for (int i = 0; i < 8; i++) {
            cp[i] = new_cp[i];
            co[i] = new_co[i] % 3;
        }
    };

    // Apply the corner transformation of the given CubieCube
    void edge_apply(CubieCube& cc) {
        int new_ep[12], new_eo[12];
        for (int i = 0; i < 12; i++) {
            new_ep[i] = ep[cc.ep[i]];
            new_eo[i] = eo[cc.ep[i]] + cc.eo[i];
        };

        for (int i = 0; i < 12; i++) {
            ep[i] = new_ep[i];
            eo[i] = new_eo[i] % 2;
        }
    };
    // Apply a transformation
    void apply(CubieCube& cc) {
        this->edge_apply(cc);
        this->corner_apply(cc);
    }

    void apply(const Move&);

    void apply(Algorithm& alg);

    bool operator==(const CubieCube& other) {
        for (uint c = 0; c < 8; ++c) {
            if (cp[c] != other.cp[c]) {
                return false;
            }
            if (co[c] != other.co[c]) {
                return false;
            }
        }
        for (uint e = 0; e < 12; ++e) {
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

        for (uint c = 0; c < 8; ++c) {
            cp[c] = other.cp[c];
            co[c] = other.co[c];
        }
        for (uint e = 0; e < 12; ++e) {
            ep[e] = other.ep[e];
            eo[e] = other.eo[e];
        }
        return *this;
    }

    int corner_parity() const {
        std::set<uint> checked;
        std::vector<uint> cycle_sizes{0};

        for (uint k = 0; k < 8; ++k) {
            while (checked.find(k) == checked.end()) {
                checked.insert(k);
                ++cycle_sizes.back();
                k = cp[k];
            }
            cycle_sizes.emplace_back(0);
        }

        uint number_of_swaps = 0;
        for (auto k : cycle_sizes) {
            if (k != 0) {
                number_of_swaps += (k - 1);
            }
        }
        return ipow(-1, number_of_swaps);
    }

    int edge_parity() const {
        std::set<uint> checked;
        std::vector<uint> cycle_sizes{0};

        for (uint k = 0; k < 12; ++k) {
            while (checked.find(k) == checked.end()) {
                checked.insert(k);
                ++cycle_sizes.back();
                k = ep[k];
            }
            cycle_sizes.emplace_back(0);
        }

        uint number_of_swaps = 0;
        for (auto k : cycle_sizes) {
            if (k != 0) {
                number_of_swaps += (k - 1);
            }
        }
        return ipow(-1, number_of_swaps);
    }

    bool has_consistent_co() const {
        uint co_sum = 0;
        for (int c = 0; c < 8; ++c) {
            co_sum += co[c];
        }
        if (co_sum % 3 != 0) {
            return false;
        }
        return true;
    }

    bool has_consistent_eo() const {
        uint eo_sum = 0;
        for (int e = 0; e < 12; ++e) {
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

    static CubieCube random_state() {
        CubieCube cube;
        std::srand(std::time(nullptr));

        std::vector<uint> corners_left{0, 1, 2, 3, 4, 5, 6, 7};
        for (auto c = 0; c < 8; ++c) {
            auto random = std::rand();
            auto next_idx = random % (8 - c);
            cube.cp[c] = corners_left[next_idx];
            corners_left.erase(corners_left.begin() + next_idx);
        }

        std::vector<uint> edges_left{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        for (auto e = 0; e < 12; ++e) {
            auto random = std::rand();
            auto next_idx = random % (12 - e);
            cube.ep[e] = edges_left[next_idx];
            edges_left.erase(edges_left.begin() + next_idx);
        }

        int counter = 0;
        for (auto c = 0; c < 7; ++c) {
            cube.co[c] = std::rand() % 3;
            counter += cube.co[c];
        }
        cube.co[7] = (3 - (counter % 3)) % 3;

        counter = 0;
        for (auto e = 0; e < 8; ++e) {
            cube.eo[e] = std::rand() % 2;
            counter += cube.eo[e];
        }
        cube.eo[11] = (2 - (counter % 2)) % 2;

        if (cube.corner_parity() != cube.edge_parity()) {
            auto a = cube.cp[0];
            auto b = cube.cp[1];
            cube.cp[1] = a;
            cube.cp[0] = b;
        }

        return cube;
    }
};

std::array<CubieCube, 25> elementary_transformations{
    CubieCube{// U
              {1, 2, 3, 0, 4, 5, 6, 7},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {1, 2, 3, 0, 4, 5, 6, 7, 8, 9, 10, 11},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// U2
              {2, 3, 0, 1, 4, 5, 6, 7},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {2, 3, 0, 1, 4, 5, 6, 7, 8, 9, 10, 11},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// U'
              {3, 0, 1, 2, 4, 5, 6, 7},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {3, 0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// D
              {0, 1, 2, 3, 7, 4, 5, 6},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {0, 1, 2, 3, 4, 5, 6, 7, 11, 8, 9, 10},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// D2
              {0, 1, 2, 3, 6, 7, 4, 5},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 8, 9},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// D'
              {0, 1, 2, 3, 5, 6, 7, 4},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 8},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// R
              {0, 5, 1, 3, 4, 6, 2, 7},
              {0, 2, 1, 0, 0, 1, 2, 0},
              {0, 5, 2, 3, 4, 9, 1, 7, 8, 6, 10, 11},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// R2
              {0, 6, 5, 3, 4, 2, 1, 7},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {0, 9, 2, 3, 4, 6, 5, 7, 8, 1, 10, 11},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{
        // R'
        {0, 2, 6, 3, 4, 1, 5, 7},
        {0, 2, 1, 0, 0, 1, 2, 0},
        {0, 6, 2, 3, 4, 1, 9, 7, 8, 5, 10, 11},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },

    CubieCube{// L
              {3, 1, 2, 7, 0, 5, 6, 4},
              {1, 0, 0, 2, 2, 0, 0, 1},
              {0, 1, 2, 7, 3, 5, 6, 11, 8, 9, 10, 4},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// L2
              {7, 1, 2, 4, 3, 5, 6, 0},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {0, 1, 2, 11, 7, 5, 6, 4, 8, 9, 10, 3},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// L'
              {4, 1, 2, 0, 7, 5, 6, 3},
              {1, 0, 0, 2, 2, 0, 0, 1},
              {0, 1, 2, 4, 11, 5, 6, 3, 8, 9, 10, 7},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// F
              {4, 0, 2, 3, 5, 1, 6, 7},
              {2, 1, 0, 0, 1, 2, 0, 0},
              {4, 1, 2, 3, 8, 0, 6, 7, 5, 9, 10, 11},
              {1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0}},

    CubieCube{
        // F2
        {5, 4, 2, 3, 1, 0, 6, 7},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {8, 1, 2, 3, 5, 4, 6, 7, 0, 9, 10, 11},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },

    CubieCube{// F'
              {1, 5, 2, 3, 0, 4, 6, 7},
              {2, 1, 0, 0, 1, 2, 0, 0},
              {5, 1, 2, 3, 0, 8, 6, 7, 4, 9, 10, 11},
              {1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0}},

    CubieCube{// B
              {0, 1, 6, 2, 4, 5, 7, 3},
              {0, 0, 2, 1, 0, 0, 1, 2},
              {0, 1, 6, 3, 4, 5, 10, 2, 8, 9, 7, 11},
              {0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0}},

    CubieCube{// B2
              {0, 1, 7, 6, 4, 5, 3, 2},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {0, 1, 10, 3, 4, 5, 7, 6, 8, 9, 2, 11},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    CubieCube{// B'
              {0, 1, 3, 7, 4, 5, 2, 6},
              {0, 0, 2, 1, 0, 0, 1, 2},
              {0, 1, 7, 3, 4, 5, 2, 10, 8, 9, 6, 11},
              {0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0}},

    // S_URF: 120° clockwise rotation around an
    // axis through the URF and DLB corners
    CubieCube{{5, 1, 0, 4, 6, 2, 3, 7},
              {2, 1, 2, 1, 1, 2, 1, 2},
              {5, 0, 4, 8, 9, 1, 3, 11, 6, 2, 7, 10},
              {0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1}},

    // S_URF2: inverse of previous
    CubieCube{{2, 1, 5, 6, 3, 0, 4, 7},
              {1, 2, 1, 2, 2, 1, 2, 1},
              {1, 5, 9, 6, 2, 0, 8, 10, 3, 4, 11, 7},
              {1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1}},

    // S_z2: The z2 cube rotation
    CubieCube{{5, 4, 7, 6, 1, 0, 3, 2},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {8, 9, 10, 11, 5, 4, 7, 6, 0, 1, 2, 3},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    // S_y: The y cube rotation
    CubieCube{{1, 2, 3, 0, 5, 6, 7, 4},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8},
              {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0}},

    // S_y2
    CubieCube{{2, 3, 0, 1, 6, 7, 4, 5},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

    // S_y3
    CubieCube{{3, 0, 1, 2, 7, 4, 5, 6},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {3, 0, 1, 2, 7, 4, 5, 6, 11, 8, 9, 10},
              {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0}},

    // S_LR: A plane symmetry about the M-slice
    CubieCube{{1, 0, 3, 2, 5, 4, 7, 6},
              {0, 0, 0, 0, 0, 0, 0, 0},
              {0, 3, 2, 1, 5, 4, 7, 6, 8, 11, 10, 9},
              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}

};

void CubieCube::apply(const Move& m) { apply(elementary_transformations[m]); }

void CubieCube::apply(Algorithm& alg) {
    for (Move& m : alg.sequence) {
        apply(m);
    }
}