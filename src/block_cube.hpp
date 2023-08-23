#pragma once
#include <array>
#include <iostream>
#include <list>

#include "coordinate.hpp"
#include "cubie_cube.hpp"

template <uint nc, uint ne>
struct Block {
    std::array<uint, nc> corners{0};  // corner indices
    std::array<uint, ne> edges{0};    // edge indices

    std::array<uint, 8> c_order{
        0, 1, 2, 3,
        4, 5, 6, 7};  // map: [0,...,7]->[block corners,non-block corners]
    std::array<uint, 12> e_order{
        0, 1, 2, 3, 4,  5,
        6, 7, 8, 9, 10, 11};  // map: [0,...,11]->[block edges,non-block edges]

    std::string name;

    Block(){};
    Block(std::string bname, const std::initializer_list<uint> bc,
          const std::initializer_list<uint> be) {
        name = bname;
        for (uint i = 0; i < nc; i++) {
            corners[i] = *(bc.begin() + i);
        }
        for (uint i = 0; i < ne; i++) {
            edges[i] = *(be.begin() + i);
        }

        std::list<uint> c{0, 1, 2, 3, 4, 5, 6, 7};
        std::list<uint> e{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        for (int i = nc - 1; i >= 0; i--) {
            c.remove(corners[i]);
            c.push_front(corners[i]);
        }
        for (int i = ne - 1; i >= 0; i--) {
            e.remove(edges[i]);
            e.push_front(edges[i]);
        }
        uint *ptr_c_order = c_order.begin();
        for (auto ptr_c = c.begin(); ptr_c != c.end(); ptr_c++) {
            *ptr_c_order = *ptr_c;
            ptr_c_order++;
        }
        uint *ptr_e_order = e_order.begin();
        for (auto ptr_e = e.begin(); ptr_e != e.end(); ptr_e++) {
            *ptr_e_order = *ptr_e;
            ptr_e_order++;
        }
    };
    void show() {
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

template <uint nc, uint ne>
struct BlockCube {
    uint cl[8]{0};     // Corner layout
    uint el[12]{0};    // Edge layout
    uint cp[nc]{0};    // Permutation of the block corners
    uint ep[ne]{0};    // Permutation of the block edges
    uint co[nc]{0};    // Orientation of the block corners
    uint eo[ne]{0};    // Orientation of the block edges
    Block<nc, ne> b;  // Block object

    BlockCube(){};
    BlockCube(Block<nc, ne> block) : b{block} {};

    void to_coordinates(uint &ccl, uint &cel, uint &ccp, uint &cep, uint &cco,
                        uint &ceo) {
        ccl = layout_coord(cl, 8);
        cel = layout_coord(el, 12);
        ccp = perm_coord(cp, nc);
        cep = perm_coord(ep, ne);
        cco = co_coord(co, nc);
        ceo = eo_coord(eo, ne);
    };

    void from_coordinates(uint ccl, uint cel, uint ccp, uint cep, uint cco,
                        uint ceo) {
        layout_from_coord(ccl, 8, nc, cl);
        perm_from_coord(ccp, nc, cp);
        co_from_coord(cco, nc, co);
        layout_from_coord(cel, 12, ne, el);
        perm_from_coord(cep, ne, ep);
        eo_from_coord(ceo, ne, eo);
    }

    void from_cubie_cube(CubieCube &cc) {
        uint kl = 0, k = 0;
        for (uint &c : b.c_order) {
            cl[kl] = 0;
            for (uint i = 0; i < nc; i++) {
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
        for (uint &e : b.e_order) {
            el[kl] = 0;
            for (uint i = 0; i < ne; i++) {
                if (cc.ep[e] == b.edges[i]) {
                    el[kl] = 1;
                    ep[k] = i;
                    eo[k] = cc.eo[e];
                    k++;
                }
            }
            kl++;
        }
    };

    template <typename CoordinateBlockCube>
    void from_coordinate_block_cube(const CoordinateBlockCube& cbc) {
        layout_from_coord(cbc.ccl, 8, nc, cl);
        perm_from_coord(cbc.ccp, nc, cp);
        co_from_coord(cbc.cco, nc, co);
        layout_from_coord(cbc.cel, 12, ne, el);
        perm_from_coord(cbc.cep, ne, ep);
        eo_from_coord(cbc.ceo, ne, eo);
    }

    void to_cubie_cube(CubieCube &cc) {
        // All pieces that do not belong to the block are set to
        // default inconsistent values:
        // 8 for CP, 12 for EP, 2 for Eo, 3 for CO
        // This means that the resulting CubieCube cannot be used as a
        // left multiplicator
        // CHECKME: Remove this function ?
        uint k = 0;
        for (uint i = 0; i < 8; i++) {
            if (cl[i] == 1) {
                cc.cp[b.c_order[i]] = b.corners[cp[k]];
                cc.co[b.c_order[i]] = co[k];
                k++;
            } else {
                cc.cp[b.c_order[i]] = 8;
                cc.co[b.c_order[i]] = 3;
            }
        }

        k = 0;
        for (uint i = 0; i < 12; i++) {
            if (el[i] == 1) {
                cc.ep[b.e_order[i]] = b.edges[ep[k]];
                cc.eo[b.e_order[i]] = eo[k];
                k++;
            } else {
                cc.ep[b.e_order[i]] = 12;
                cc.eo[b.e_order[i]] = 2;
            }
        }
    }

    void show() {
        std::cout << "BlockCube object: " << '\n';
        std::cout << "  ";
        b.show();
        std::cout << "  CL: ";
        for (uint i = 0; i < 8; i++) {
            std::cout << cl[i] << ' ';
        }
        std::cout << '\n';
        std::cout << "  CP: ";
        for (uint i = 0; i < nc; i++) {
            std::cout << cp[i] << ' ';
        }
        std::cout << '\n';
        std::cout << "  CO: ";
        for (uint i = 0; i < nc; i++) {
            std::cout << co[i] << ' ';
        }
        std::cout << '\n';
        std::cout << "  EL: ";
        for (uint i = 0; i < 12; i++) {
            std::cout << el[i] << ' ';
        }
        std::cout << '\n';
        std::cout << "  EP: ";
        for (uint i = 0; i < ne; i++) {
            std::cout << ep[i] << ' ';
        }
        std::cout << '\n';
        std::cout << "  EO: ";
        for (uint i = 0; i < ne; i++) {
            std::cout << eo[i] << ' ';
        }
        std::cout << '\n';
    }
};
