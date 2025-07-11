#include "pruning_table.hpp"

#include "block.hpp"
#include "cubie_cube.hpp"
#include "move_table.hpp"

template <unsigned nc, unsigned ne>
void test_generate(Block<nc, ne>& b) {
    auto mtable = BlockMoveTable(b);
    auto apply = mtable.get_apply();

    auto index = [&b](const CoordinateBlockCube& cbc) {
        unsigned ci = cbc.ccl * b.n_cp * b.n_co + (cbc.ccp * b.n_co + cbc.cco);
        unsigned ei = cbc.cel * b.n_ep * b.n_eo + (cbc.cep * b.n_eo + cbc.ceo);
        return ei * b.n_cs + ci;
    };

    constexpr size_t table_size = b.n_es * b.n_cs;
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