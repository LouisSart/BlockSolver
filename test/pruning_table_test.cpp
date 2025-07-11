#include "pruning_table.hpp"

#include "block.hpp"
#include "cubie_cube.hpp"
#include "move_table.hpp"

void test_generate() {
    auto b = Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB});
    auto mtable = BlockMoveTable(b);
    auto apply = mtable.get_apply();

    auto index = [&b](const CoordinateBlockCube& cbc) {
        unsigned ci = cbc.ccl * b.n_cp * b.n_co + (cbc.ccp * b.n_co + cbc.cco);
        unsigned ei = cbc.cel * b.n_ep * b.n_eo + (cbc.cep * b.n_eo + cbc.ceo);
        return ei * b.n_cs + ci;
    };

    constexpr size_t table_size = b.n_es * b.n_cs;
    PruningTable<table_size> ptable;
    ptable.generate<CoordinateBlockCube>(apply, index);

    assert(ptable.is_filled());

    ptable.write("pruning_table/222_table.dat");
    PruningTable<table_size> reload;
    reload.load("pruning_table/222_table.dat");

    assert(reload.is_filled());
    for (unsigned i = 0; i < table_size; ++i) {
        assert(ptable.estimate(i) == reload.estimate(i));
    }
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

    ptable.write("pruning_table/eo_table.dat");
    PruningTable<table_size> reload;
    reload.load("pruning_table/eo_table.dat");

    assert(reload.is_filled());
    for (unsigned i = 0; i < table_size; ++i) {
        assert(ptable.estimate(i) == reload.estimate(i));
    }
}

int main() {
    test_generate();
    test_EO_generate();
    return 0;
}