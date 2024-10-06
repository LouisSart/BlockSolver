#pragma once
#include <array>
#include <iostream>

struct CoordinateBlockCube {
    // Coordinates for cp, co, ep, eo, corner layout and edge layout
    unsigned ccl, cel, ccp, cep, cco, ceo;

    CoordinateBlockCube() : ccl{0}, cel{0}, ccp{0}, cep{0}, cco{0}, ceo{0} {};

    CoordinateBlockCube(unsigned ccl, unsigned cel, unsigned ccp, unsigned cep,
                        unsigned cco, unsigned ceo)
        : ccl{ccl}, cel{cel}, ccp{ccp}, cep{cep}, cco{cco}, ceo{ceo} {};

    void set(unsigned ccl_in, unsigned cel_in, unsigned ccp_in, unsigned cep_in,
             unsigned cco_in, unsigned ceo_in) {
        ccl = ccl_in;
        cel = cel_in;
        ccp = ccp_in;
        cep = cep_in;
        cco = cco_in;
        ceo = ceo_in;
    };

    bool operator==(const CoordinateBlockCube& other) {
        return (ccl == other.ccl && cel == other.cel && ccp == other.ccp &&
                cep == other.cep && cco == other.cco && ceo == other.ceo);
    }

    void show() const {
        std::cout << "CoordinateBlockCube:\n";
        std::cout << " Corner layout coordinate: " << ccl << '\n';
        std::cout << " CP coordinate: " << ccp << '\n';
        std::cout << " CO coordinate: " << cco << '\n';
        std::cout << " Edge layout coordinate: " << cel << '\n';
        std::cout << " EP coordinate: " << cep << '\n';
        std::cout << " EO coordinate: " << ceo << '\n';
    };
};

template <unsigned nb>
struct MultiBlockCube : std::array<CoordinateBlockCube, nb> {
    void show() const {
        std::cout << "MultiBlockCube<" << nb << ">" << std::endl;
        for (unsigned k = 0; k < nb; ++k) {
            std::cout << "   Block " << k << ": ";
            if ((*this)[k].is_solved()) {
                std ::cout << "solved" << std::endl;
            } else {
                std ::cout << "not solved" << std::endl;
            }
        }
    }
};