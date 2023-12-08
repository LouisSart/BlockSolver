#pragma once
#include <array>
#include <iostream>

using uint = unsigned int;

struct CoordinateBlockCube {
    uint ccl, cel, ccp, cep, cco,
        ceo;  // Coordinates for cp, co, ep, eo, corner layout and edge layout

    CoordinateBlockCube() : ccl{0}, cel{0}, ccp{}, cep{0}, cco{0}, ceo{0} {};

    CoordinateBlockCube(uint ccl, uint cel, uint ccp, uint cep, uint cco,
                        uint ceo)
        : ccl{ccl}, cel{cel}, ccp{ccp}, cep{cep}, cco{cco}, ceo{ceo} {};

    void set(uint ccl_in, uint cel_in, uint ccp_in, uint cep_in, uint cco_in,
             uint ceo_in) {
        ccl = ccl_in;
        cel = cel_in;
        ccp = ccp_in;
        cep = cep_in;
        cco = cco_in;
        ceo = ceo_in;
    };

    bool operator==(const CoordinateBlockCube &other) {
        return (ccl == other.ccl && cel == other.cel && ccp == other.ccp &&
                cep == other.cep && cco == other.cco && ceo == other.ceo);
    }

    bool is_solved() const {
        return (ccl == 0 && cel == 0 && ccp == 0 && cep == 0 && cco == 0 &&
                ceo == 0);
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