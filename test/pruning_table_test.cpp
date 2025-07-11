#include "pruning_table.hpp"

#include "block.hpp"
#include "cubie_cube.hpp"
#include "move_table.hpp"

void test_generate() {
    auto b = Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB});
    auto mtable = BlockMoveTable(b);

    constexpr size_t table_size = b.n_es * b.n_cs;
    PruningTable<table_size> ptable;
    ptable.generate<CoordinateBlockCube>(mtable.get_apply(), b.get_indexer());

    assert(ptable.is_filled());

    ptable.write(b.id);
    PruningTable<table_size> reload;
    reload.load(b.id);

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

    ptable.write("eo");
    PruningTable<table_size> reload;
    reload.load("eo");

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