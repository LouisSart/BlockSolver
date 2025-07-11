#include "pruning_table.hpp"

#include "block.hpp"
#include "cubie_cube.hpp"
#include "move_table.hpp"

template <unsigned nc, unsigned ne>
void test_generate(Block<nc, ne>& block) {
    auto mtable = BlockMoveTable(block);
    auto apply = mtable.get_apply();

    constexpr unsigned n_cp = factorial(nc);
    constexpr unsigned n_co = ipow(3, nc);
    constexpr unsigned n_ep = factorial(ne);
    constexpr unsigned n_eo = ipow(2, ne);
    constexpr size_t n_corner_states =
        (factorial(8) / factorial(8 - nc)) * ipow(3, nc);
    constexpr size_t n_edge_states =
        (factorial(12) / factorial(12 - ne)) * ipow(2, ne);
    constexpr size_t table_size = n_corner_states * n_edge_states;
    auto index = [](const CoordinateBlockCube& cbc) {
        unsigned ci = cbc.ccl * n_cp * n_co + (cbc.ccp * n_co + cbc.cco);
        unsigned ei = cbc.cel * n_ep * n_eo + (cbc.cep * n_eo + cbc.ceo);
        return ei * n_corner_states + ci;
    };

    PruningTable<table_size> ptable;
    // Ici le mot-clé template est nécessaire car nous sommes dans une
    // fonction elle-même template. Le compilateur ne sait pas encore
    // à ce stade que la fonction generate est templatée !
    ptable.template generate<CoordinateBlockCube>(apply, index);

    assert(ptable.is_filled());
}

void test_EO_generate() {
    auto mtable = EOMoveTable();
    auto apply = mtable.get_apply();

    constexpr unsigned n_eo = ipow(2, NE - 1);
    constexpr size_t table_size = n_eo;
    auto index = [](const CoordinateBlockCube& cbc) { return cbc.ceo; };

    PruningTable<table_size> ptable;
    ptable.generate<CoordinateBlockCube>(apply, index);

    assert(ptable.is_filled());
}

int main() {
    auto DLB_222 = Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB});
    test_generate(DLB_222);
    test_EO_generate();
    return 0;
}