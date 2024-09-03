#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>

#include "coordinate.hpp"
#include "coordinate_block_cube.hpp"
#include "cubie_cube.hpp"
#include "utils.hpp"

template <unsigned nc, unsigned ne>
struct Block {
    std::array<Corner, nc> corners;  // corner indices
    std::array<Edge, ne> edges;      // edge indices

    std::string name;
    std::string id;

    Block() {}
    // Block(std::string bname, const std::array<Corner, nc> &bc,
    //       const std::array<Edge, ne> &be)
    //     : corners{bc}, edges{be}, name{bname} {

    // }
    Block(std::string n, const std::array<Corner, nc> &c,
          const std::array<Edge, ne> &e)
        : name{n}, corners{c}, edges{e} {
        assert(c.size() == nc);
        assert(e.size() == ne);

        // Sort corners and edges for unicity
        std::sort(corners.begin(), corners.end(),
                  [](const Cubie &c1, const Cubie &c2) { return (c1 < c2); });
        std::sort(edges.begin(), edges.end(),
                  [](const Cubie &e1, const Cubie &e2) { return (e1 < e2); });
        id = compute_id();
    };

    std::string compute_id() const {
        std::array<unsigned, NC> corner_positions{0};
        std::array<unsigned, NE> edge_positions{0};

        for (auto c : corners) {
            corner_positions[c] = 1;
        }
        for (auto e : edges) {
            edge_positions[e] = 1;
        }

        auto c_id = layout_index(corner_positions, nc);
        auto e_id = layout_index(edge_positions, ne);

        // Block ID is of the form: ncCcid_neEeid
        // For example the DLB 2x2x2 has ID: 1C7_3E217
        return std::to_string(nc) + "C" + std::to_string(c_id) + "_" +
               std::to_string(ne) + "E" + std::to_string(e_id);
    }

    bool c_contains(const Cubie &c) const {
        return (std::find(corners.begin(), corners.end(), c) !=
                std::end(corners));
    }

    bool e_contains(const Cubie &e) const {
        return (std::find(edges.begin(), edges.end(), e) != std::end(edges));
    }

    void show() const {
        std::cout << "Block<" << nc << ", " << ne << ">";
        std::cout << " \"" << name << "\"";
        std::cout << "\n   Corners: {";
        for (auto &c : corners) {
            std::cout << c << ' ';
        }
        if (nc > 0) std::cout << "\b}";
        std::cout << "\n   Edges: {";
        for (auto &e : edges) {
            std::cout << e << ' ';
        }
        if (ne > 0) std::cout << "\b}";
        std::cout << std::endl;
    };
};

template <unsigned nc, unsigned ne>
struct BlockCube {
    // This struct is responsible for all conversions between CubieCube and
    // CoordinateBlockCube because we want to avoid reciprocal dependency
    // between CubieCube and CoordinateBlockCube

    std::array<unsigned, NC> cl;  // Corner layout
    std::array<unsigned, NE> el;  // Edge layout
    std::array<unsigned, nc> cp;  // Permutation of the block corners
    std::array<unsigned, ne> ep;  // Permutation of the block edges
    std::array<unsigned, nc> co;  // Orientation of the block corners
    std::array<unsigned, ne> eo;  // Orientation of the block edges
    Block<nc, ne> b;              // Block object

    BlockCube(){};
    BlockCube(std::string bname, const std::array<Corner, nc> bc,
              const std::array<Edge, ne> be)
        : b(bname, bc, be){};
    BlockCube(const Block<nc, ne> &b) : b(b){};

    CoordinateBlockCube to_coordinate_block_cube(const CubieCube &cc) {
        // Returns the coordinate representation
        // of the block state in the input CubieCube

        CoordinateBlockCube cbc;
        unsigned k = 0;
        for (Cubie c = ULF; c <= DLB; ++c) {
            if (b.c_contains(cc.cp[c])) {
                assert(k < nc);
                assert(c < NC);
                cl[c] = 1;
                cp[k] = cc.cp[c];
                co[k] = cc.co[c];
                ++k;
            } else {
                cl[c] = 0;
            }
        }

        k = 0;
        for (Cubie e = UF; e <= DL; ++e) {
            if (b.e_contains(cc.ep[e])) {
                assert(k < ne);
                assert(e < NE);
                el[e] = 1;
                ep[k] = cc.ep[e];
                eo[k] = cc.eo[e];
                k++;
            } else {
                el[e] = 0;
            }
        }

        if constexpr (nc > 0) {
            cbc.ccl = layout_index(cl, nc);
            cbc.ccp = permutation_index(cp);
            cbc.cco = co_index(co);
        };
        if constexpr (ne > 0) {
            cbc.cel = layout_index(el, ne);
            cbc.cep = permutation_index(ep);
            cbc.ceo = eo_index(eo);
        };

        return cbc;
    }

    CubieCube to_cubie_cube(const CoordinateBlockCube &cbc) {
        // Takes in a CoordinateBlockCube and returns the corresponding
        // CubieCube. All pieces that do not belong to the block are set to
        // default inconsistent values:
        // 8 for CP, 12 for EP, 2 for EO, 3 for CO
        // This means that the resulting CubieCube cannot be used as a
        // right multiplicator

        CubieCube cc;
        unsigned k;
        if constexpr (nc > 0) {
            layout_from_index(cbc.ccl, cl, nc);
            permutation_from_index(cbc.ccp, cp);
            co_from_index(cbc.cco, co);
            k = 0;
            for (Cubie i = ULF; i <= DLB; ++i) {
                assert(k < NC);
                if (cl[i] == 1) {
                    cc.cp[i] = b.corners[cp[k]];
                    cc.co[i] = co[k];
                    ++k;
                } else {
                    cc.cp[i] = NC;
                    cc.co[i] = 3;
                }
            }
        };

        if constexpr (ne > 0) {
            layout_from_index(cbc.cel, el, ne);
            permutation_from_index(cbc.cep, ep);
            eo_from_index(cbc.ceo, eo);

            k = 0;
            for (Cubie i = 0; i < NE; i++) {
                assert(k < NE);
                if (el[i] == 1) {
                    cc.ep[i] = b.edges[ep[k]];
                    cc.eo[i] = eo[k];
                    k++;
                } else {
                    cc.ep[i] = NE;
                    cc.eo[i] = 2;
                }
            }
        };
        return cc;
    }

    void show() {
        std::cout << "BlockCube object: " << '\n';
        std::cout << "  ";
        b.show();
        std::cout << "  CL: ";
        for (Cubie i = 0; i < NC; ++i) {
            std::cout << cl[i] << ' ';
        }
        std::cout << '\n';
        std::cout << "  CP: ";
        for (Cubie i = 0; i < nc; ++i) {
            std::cout << cp[i] << ' ';
        }
        std::cout << '\n';
        std::cout << "  CO: ";
        for (Cubie i = 0; i < nc; ++i) {
            std::cout << co[i] << ' ';
        }
        std::cout << '\n';
        std::cout << "  EL: ";
        for (Cubie i = 0; i < NE; ++i) {
            std::cout << el[i] << ' ';
        }
        std::cout << '\n';
        std::cout << "  EP: ";
        for (Cubie i = 0; i < ne; ++i) {
            std::cout << ep[i] << ' ';
        }
        std::cout << '\n';
        std::cout << "  EO: ";
        for (Cubie i = 0; i < ne; ++i) {
            std::cout << eo[i] << ' ';
        }
        std::cout << '\n';
    }
};
