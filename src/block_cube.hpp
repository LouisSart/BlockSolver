#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>

#include "coordinate.hpp"
#include "coordinate_block_cube.hpp"
#include "cubie_cube.hpp"

template <unsigned nc, unsigned ne>
struct Block {
    std::array<Corner, nc> corners;  // corner indices
    std::array<Edge, ne> edges;      // edge indices

    // map: [0,...,7]->[block corners,non-block corners]
    std::array<Corner, NC> c_order;
    // map: [0,...,11]->[block edges,non-block edges]
    std::array<Edge, NE> e_order;

    std::string name;
    std::string id;

    Block() {}
    Block(std::string bname, const std::array<Corner, nc> &bc,
          const std::array<Edge, ne> &be)
        : corners{bc}, edges{be}, name{bname} {
        // Sort corners and edges for unicity
        std::sort(corners.begin(), corners.end(),
                  [](const Cubie &c1, const Cubie &c2) { return (c1 < c2); });
        std::sort(edges.begin(), edges.end(),
                  [](const Cubie &e1, const Cubie &e2) { return (e1 < e2); });

        // Compute the run through order of the pieces. Needed for correct
        // computation of corner and edges layout/permutation coordinates
        std::list<Corner> c{ULF, URF, URB, ULB, DLF, DRF, DRB, DLB};
        std::list<Edge> e{UF, UR, UB, UL, LF, RF, RB, LB, DF, DR, DB, DL};
        for (int i = nc - 1; i >= 0; i--) {  // has to be int because of i >= 0
            assert((corners[i] >= ULF) && (corners[i] <= DLB));
            c.remove(corners[i]);
            c.push_front(corners[i]);
        }
        for (int i = ne - 1; i >= 0; i--) {  // has to be int because of i >= 0
            assert((edges[i] >= UF) && (edges[i] <= DL));
            e.remove(edges[i]);
            e.push_front(edges[i]);
        }
        Corner *ptr_c_order = c_order.begin();
        for (auto ptr_c = c.begin(); ptr_c != c.end(); ptr_c++) {
            *ptr_c_order = *ptr_c;
            ptr_c_order++;
        }
        Edge *ptr_e_order = e_order.begin();
        for (auto ptr_e = e.begin(); ptr_e != e.end(); ptr_e++) {
            *ptr_e_order = *ptr_e;
            ptr_e_order++;
        }
        id = compute_id();
    }

    Block(std::string bname, const std::initializer_list<Corner> bc,
          const std::initializer_list<Edge> be) {
        assert(bc.size() == nc);
        assert(be.size() == ne);

        std::array<Corner, nc> car;
        std::array<Edge, ne> ear;
        auto lit = bc.begin();
        for (unsigned k = 0; k < nc; ++k) {
            assert((ULF <= *lit) && (*lit <= DLB));
            car[k] = *lit;
            ++lit;
        }
        auto lit_e = be.begin();
        for (unsigned k = 0; k < ne; ++k) {
            assert((UF <= *lit_e) && (*lit_e <= DL));
            ear[k] = *lit_e;
            ++lit_e;
        }
        *this = Block(bname, car, ear);
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

        auto c_id = layout_coord(corner_positions.data(), NC);
        auto e_id = layout_coord(edge_positions.data(), NE);

        // Block ID is of the form: ncCcid_neEeid
        // For example the DLB 2x2x2 has ID: 1C7_3E217
        return std::to_string(nc) + "C" + std::to_string(c_id) + "_" +
               std::to_string(ne) + "E" + std::to_string(e_id);
    }

    void show() const {
        std::cout << "Block<" << nc << ", " << ne << ">";
        std::cout << " \"" << name << "\"";
        std::cout << " c{";
        for (auto &c : corners) {
            std::cout << c << ' ';
        }
        std::cout << "} e{";
        for (auto &e : edges) {
            std::cout << e << ' ';
        }
        std::cout << "}" << std::endl;
        std::cout << "   Corner order: ";
        for (auto &c : c_order) {
            std::cout << c << ' ';
        }
        std::cout << std::endl;
        std::cout << "   Edge order: ";
        for (auto &e : e_order) {
            std::cout << e << ' ';
        }
        std::cout << std::endl;
    };

    std::tuple<Block<nc, 0>, Block<0, ne>> split_corners_and_edges() const {
        Block<nc, 0> corner_part(name + "_corners", corners,
                                 std::array<Edge, 0>{});
        Block<0, ne> edge_part(name + "_edges", std::array<Corner, 0>{}, edges);
        return std::tuple(corner_part, edge_part);
    }
};

template <unsigned nc, unsigned ne>
struct BlockCube {
    // This struct is responsible for all conversions between CubieCube and
    // CoordinateBlockCube because we want to avoid reciprocal dependency
    // between CubieCube and CoordinateBlockCube

    unsigned cl[NC];  // Corner layout
    unsigned el[NE];  // Edge layout
    unsigned cp[nc];  // Permutation of the block corners
    unsigned ep[ne];  // Permutation of the block edges
    unsigned co[nc];  // Orientation of the block corners
    unsigned eo[ne];  // Orientation of the block edges
    Block<nc, ne> b;  // Block object

    BlockCube(){};
    BlockCube(std::string bname, const std::initializer_list<Corner> bc,
              const std::initializer_list<Edge> be)
        : b(bname, bc, be){};
    BlockCube(const Block<nc, ne> &b) : b(b){};

    CoordinateBlockCube to_coordinate_block_cube(const CubieCube &cc) {
        // Returns the coordinate representation
        // of the block state in the input CubieCube

        CoordinateBlockCube cbc;
        unsigned kl = 0, k = 0;
        for (auto &c : b.c_order) {
            cl[kl] = 0;
            for (Cubie i = 0; i < nc; i++) {
                if (cc.cp[c] == b.corners[i]) {
                    cl[kl] = 1;
                    cp[k] = i;
                    co[k] = cc.co[c];
                    k++;
                }
            }
            kl++;
        }

        kl = 0, k = 0;
        for (auto &e : b.e_order) {
            el[kl] = 0;
            for (Cubie i = 0; i < ne; i++) {
                if (cc.ep[e] == b.edges[i]) {
                    assert(k < 12);
                    assert(kl < 12);
                    el[kl] = 1;
                    ep[k] = i;
                    eo[k] = cc.eo[e];
                    k++;
                }
            }
            kl++;
        }

        if constexpr (nc > 0) {
            cbc.ccl = layout_coord(cl, NC);
            cbc.ccp = perm_coord(cp, nc);
            cbc.cco = co_coord(co, nc);
        };
        if constexpr (ne > 0) {
            cbc.cel = layout_coord(el, NE);
            cbc.cep = perm_coord(ep, ne);
            cbc.ceo = eo_coord(eo, ne);
        };

        return cbc;
    }

    CubieCube to_cubie_cube(const CoordinateBlockCube &cbc) {
        // Takes in a CoordinateBlockCube and returns the corresponding
        // CubieCube All pieces that do not belong to the block are set to
        // default inconsistent values:
        // 8 for CP, 12 for EP, 2 for Eo, 3 for CO
        // This means that the resulting CubieCube cannot be used as a
        // left multiplicator

        CubieCube cc;
        unsigned k;
        if constexpr (nc > 0) {
            layout_from_coord(cbc.ccl, NC, nc, cl);
            perm_from_coord(cbc.ccp, nc, cp);
            co_from_coord(cbc.cco, nc, co);
            k = 0;
            for (Cubie i = 0; i < NC; ++i) {
                assert(k < NC);
                if (cl[i] == 1) {
                    cc.cp[b.c_order[i]] = b.corners[cp[k]];
                    cc.co[b.c_order[i]] = co[k];
                    k++;
                } else {
                    cc.cp[b.c_order[i]] = NC;
                    cc.co[b.c_order[i]] = 3;
                }
            }
        };

        if constexpr (ne > 0) {
            layout_from_coord(cbc.cel, NE, ne, el);
            perm_from_coord(cbc.cep, ne, ep);
            eo_from_coord(cbc.ceo, ne, eo);

            k = 0;
            for (Cubie i = 0; i < NE; i++) {
                assert(k < NE);
                if (el[i] == 1) {
                    cc.ep[b.e_order[i]] = b.edges[ep[k]];
                    cc.eo[b.e_order[i]] = eo[k];
                    k++;
                } else {
                    cc.ep[b.e_order[i]] = NE;
                    cc.eo[b.e_order[i]] = 2;
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
