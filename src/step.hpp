#include <vector>

#include "algorithm.hpp"
#include "move_table.hpp"

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
