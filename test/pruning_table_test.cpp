#include "pruning_table.hpp"

template <typename Block>
void test_api(Block& b) {
    auto cc = CubieCube::random_state();
    auto cbc = b.to_coordinate_block_cube(cc);
    auto strat = Strategy::Optimal(b);
    auto table = strat.gen_table();
    auto pruning_value = table.get_estimate(cbc);

    assert(pruning_value != 0);
    assert(strat.from_index(strat.index(cbc)) == cbc);
}

template <unsigned nc, unsigned ne>
void test_reload(Block<nc, ne>& b) {
    auto table = Strategy::Optimal(b).gen_table();
    table.write();
    auto reloaded = Strategy::Optimal(b).load_table();

    for (int k = 0; k < table.size(); ++k) {
        assert(table.table[k] == reloaded.table[k]);
    }
}

template <typename Block>
void test_table_is_correct(Block& b) {
    auto table = Strategy::Optimal(b).gen_table();
    for (int i = 0; i < table.size(); ++i) {
        assert(table.table[i] != 255);
    }
}

template <typename Block>
void test_direct_and_backward_are_equivalent(Block& b) {
    Strategy::Optimal strat(b);
    BlockMoveTable m_table(b);
    auto direct = load_table_ptr(strat);
    auto backwards = load_table_ptr(strat);

    std::cout << "Direct generator:" << std::endl;
    auto adv = Advancement(direct->size());
    compute_pruning_table<false>(*direct, strat, m_table, adv);
    adv.update(adv.depth + 1);

    std::cout << "Backwards generator:" << std::endl;
    adv = Advancement(backwards->size());
    adv.add_encountered();
    adv.update();
    backwards->reset();
    backwards->table[0] = 0;
    compute_pruning_table_backwards(*backwards, strat, m_table, adv);

    for (unsigned k = 0; k < direct->size(); ++k) {
        assert(backwards->table[k] == direct->table[k]);
    }
    direct->write();
}

void test_eo_table() {
    Strategy::OptimalEO strat;
    EOMoveTable m_table;
    auto table = strat.gen_table();
    auto table_check = strat.load_table();

    for (unsigned k = 0; k < table.size(); ++k) {
        assert(table[k] == table_check[k]);
    }
}

int main() {
    auto LF_column = Block<2, 1>("LF_column", {ULF, DLB}, {LF});
    auto DLB_222 = Block<1, 3>("DLB_222", {DLB}, {DL, LB, DB});
    auto Roux_FB = Block<2, 3>("RouxFirstBlock", {DLF, DLB}, {LF, DL, LB});
    auto DL_223 = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});

    std::cout << "API tests" << std::endl;
    test_api(LF_column);

    std::cout << "\nReload tests" << std::endl;
    test_reload(DLB_222);

    std::cout << "\nCorrectness tests" << std::endl;
    test_table_is_correct(DLB_222);

    std::cout << "\nDirect and backwards equivalence test" << std::endl;
    test_direct_and_backward_are_equivalent(LF_column);
    test_direct_and_backward_are_equivalent(DLB_222);

    std::cout << "\nEO table test" << std::endl;
    test_eo_table();
    return 0;
}