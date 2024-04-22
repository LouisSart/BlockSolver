#include "pruning_table.hpp"

template <typename Block>
void test_optimal_api(const Block& b) {
    std::cout << b.name << std::endl;
    BlockCube bc(b);
    auto cc = CubieCube::random_state();
    auto cbc = bc.to_coordinate_block_cube(cc);
    auto strat = Strategy::Optimal(b);
    auto table = strat.gen_table();
    auto pruning_value = table.get_estimate(cbc);

    assert(pruning_value != 0);
    assert(strat.from_index(strat.index(cbc)) == cbc);
}

template <typename Block>
void test_permutation_api(const Block& b) {
    std::cout << b.name << std::endl;
    BlockCube bc(b);
    auto cc = CubieCube::random_state();
    auto cbc = bc.to_coordinate_block_cube(cc);

    auto strat = Strategy::Permutation(b);
    auto table = strat.gen_table();
    auto pruning_value = table.get_estimate(cbc);

    auto from_table = strat.from_index(strat.index(cbc));
    assert(from_table.ccl == cbc.ccl);
    assert(from_table.ccp == cbc.ccp);
    assert(from_table.cel == cbc.cel);
    assert(from_table.cep == cbc.cep);
}

template <unsigned nc, unsigned ne>
void test_optimal_reload(const Block<nc, ne>& b) {
    std::cout << b.name << std::endl;
    auto table = Strategy::Optimal(b).gen_table();
    table.write();
    auto reloaded = Strategy::Optimal(b).load_table();

    for (int k = 0; k < table.size(); ++k) {
        assert(table.table[k] == reloaded.table[k]);
    }
}

template <unsigned nc, unsigned ne>
void test_permutation_reload(const Block<nc, ne>& b) {
    std::cout << b.name << std::endl;
    auto table = Strategy::Permutation(b).gen_table();
    table.write();

    auto reloaded = Strategy::Permutation(b).load_table();

    for (int k = 0; k < table.size(); ++k) {
        assert(table.table[k] == reloaded.table[k]);
    }
}

template <typename Block>
void test_optimal_is_correct(const Block& b) {
    std::cout << b.name << std::endl;
    auto table = Strategy::Optimal(b).gen_table();
    for (int i = 0; i < table.size(); ++i) {
        assert(table.table[i] != 255);
    }
}

template <typename Block>
void test_permutation_is_correct(const Block& b) {
    std::cout << b.name << std::endl;
    auto table = Strategy::Permutation(b).gen_table();
    for (int i = 0; i < table.size(); ++i) {
        assert(table.table[i] != 255);
    }
}

template <typename Block>
void test_direct_and_backward_are_equivalent(const Block& b) {
    b.show();
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
    test_optimal_api(LF_column);
    test_permutation_api(LF_column);

    std::cout << "\nReload tests" << std::endl;
    test_optimal_reload(DLB_222);
    test_permutation_reload(Roux_FB);

    std::cout << "\nCorrectness tests" << std::endl;
    test_optimal_is_correct(DLB_222);
    test_permutation_is_correct(Roux_FB);

    std::cout << "\nDirect and backwards equivalence test" << std::endl;
    test_direct_and_backward_are_equivalent(LF_column);
    test_direct_and_backward_are_equivalent(DLB_222);

    std::cout << "\nEO table test" << std::endl;
    test_eo_table();
    return 0;
}