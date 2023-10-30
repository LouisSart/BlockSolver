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
    std::array<Cubie, nc> corners;  // corner indices
    std::array<Cubie, ne> edges;    // edge indices

    // map: [0,...,7]->[block corners,non-block corners]
    std::array<Cubie, NC> c_order;
    // map: [0,...,11]->[block edges,non-block edges]
    std::array<Cubie, NE> e_order;

    std::string name;
    std::string id;

    Block(std::string bname, const std::initializer_list<Cubie> bc,
          const std::initializer_list<Cubie> be)
        : name{bname} {
        assert(bc.size() == nc);
        assert(be.size() == ne);

        auto lit = bc.begin();
        for (unsigned k = 0; k < nc; ++k) {
            corners[k] = *lit;
            ++lit;
        }
        lit = be.begin();
        for (unsigned k = 0; k < ne; ++k) {
            edges[k] = *lit;
            ++lit;
        }

        // Sort corners and edges for unicity
        std::sort(corners.begin(), corners.end(),
                  [](const Cubie &c1, const Cubie &c2) { return (c1 < c2); });
        std::sort(edges.begin(), edges.end(),
                  [](const Cubie &e1, const Cubie &e2) { return (e1 < e2); });

        // Compute the run through order of the pieces. Needed for correct
        // computation of corner and edges layout/permutation coordinates
        std::list<Cubie> c{ULF, URF, URB, ULB, DLF, DRF, DRB, DLB};
        std::list<Cubie> e{UF, UR, UB, UL, LF, RF, RB, LB, DF, DR, DB, DL};
        for (int i = nc - 1; i >= 0; i--) {  // has to be int because of i >= 0
            c.remove(corners[i]);
            c.push_front(corners[i]);
        }
        for (int i = ne - 1; i >= 0; i--) {  // has to be int because of i >= 0
            e.remove(edges[i]);
            e.push_front(edges[i]);
        }
        Cubie *ptr_c_order = c_order.begin();
        for (auto ptr_c = c.begin(); ptr_c != c.end(); ptr_c++) {
            *ptr_c_order = *ptr_c;
            ptr_c_order++;
        }
        Cubie *ptr_e_order = e_order.begin();
        for (auto ptr_e = e.begin(); ptr_e != e.end(); ptr_e++) {
            *ptr_e_order = *ptr_e;
            ptr_e_order++;
        }
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
        std::cout << "}\n";
    };
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
    BlockCube(std::string bname, const std::initializer_list<Cubie> bc,
              const std::initializer_list<Cubie> be)
        : b(bname, bc, be){};
    BlockCube(Block<nc, ne> b) : b(b){};

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

        cbc.ccl = layout_coord(cl, NC);
        cbc.cel = layout_coord(el, NE);
        cbc.ccp = perm_coord(cp, nc);
        cbc.cep = perm_coord(ep, ne);
        cbc.cco = co_coord(co, nc);
        cbc.ceo = eo_coord(eo, ne);

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
        layout_from_coord(cbc.ccl, NC, nc, cl);
        perm_from_coord(cbc.ccp, nc, cp);
        co_from_coord(cbc.cco, nc, co);
        layout_from_coord(cbc.cel, NE, ne, el);
        perm_from_coord(cbc.cep, ne, ep);
        eo_from_coord(cbc.ceo, ne, eo);

        unsigned k = 0;
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
