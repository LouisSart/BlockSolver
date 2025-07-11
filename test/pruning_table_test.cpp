#include "pruning_table.hpp"

#include "block.hpp"
#include "cubie_cube.hpp"
#include "move_table.hpp"

// template <typename Block>
// void test_api(Block& b) {
//     auto cc = CubieCube::random_state();
//     auto cbc = b.to_coordinate_block_cube(cc);
//     auto strat = Strategy::Optimal(b);
//     auto table = generate_pruning_table(b);
//     auto pruning_value = table.get_estimate(cbc);

//     assert(pruning_value != 0);
//     assert(strat.from_index(strat.index(cbc)) == cbc);
// }

// template <unsigned nc, unsigned ne>
// void test_reload(Block<nc, ne>& b) {
//     auto table = generate_pruning_table(b);
//     table.write();
//     auto reloaded = load_pruning_table(b);

//     for (int k = 0; k < table.size(); ++k) {
//         assert(table.table[k] == reloaded.table[k]);
//     }
// }

// template <typename Block>
// void test_table_is_correct(Block& b) {
//     auto table = generate_pruning_table(b);
//     for (int i = 0; i < table.size(); ++i) {
//         assert(table.table[i] != 255);
//     }
// }

// template <typename Block>
// void test_direct_and_backward_are_equivalent(Block& b) {
//     BlockMoveTable m_table(b);
//     auto direct = load_pruning_table(b);
//     auto backwards = load_pruning_table(b);

//     std::cout << "Direct generator:" << std::endl;
//     Strategy::Optimal strat(b);
//     auto adv = Advancement(direct.size());
//     compute_pruning_table<false>(direct, strat, m_table, adv);
//     adv.update(adv.depth + 1);

//     std::cout << "Backwards generator:" << std::endl;
//     adv = Advancement(backwards.size());
//     adv.add_encountered();
//     adv.update();
//     backwards.reset();
//     backwards.table[strat.get_index_of_solved()] = 0;
//     compute_pruning_table_backwards(backwards, strat, m_table, adv);

//     for (unsigned k = 0; k < direct.size(); ++k) {
//         assert(backwards.table[k] == direct.table[k]);
//     }
//     direct.write();
// }

// void test_eo_table() {
//     EOMoveTable m_table;
//     auto table = generate_eo_table();
//     auto table_check = load_eo_table();

//     for (unsigned k = 0; k < table.size(); ++k) {
//         assert(table[k] == table_check[k]);
//     }
// }

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
    ptable.template generate<CoordinateBlockCube>(apply, index);

    assert(ptable.is_filled());
}

int main() {
    auto DLB_222 = Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB});
    test_generate(DLB_222);
    test_EO_generate();
    return 0;
}