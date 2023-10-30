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
    Strategy::Permutation strat(b);
    auto direct = PruningTable(strat);
    auto backwards = PruningTable(strat);

    std::cout << "Direct generator:" << std::endl;
    auto adv = Advancement(direct.size());
    compute_pruning_table<false>(direct, strat, adv);
    adv.update(adv.depth + 1);

    std::cout << "Backwards generator:" << std::endl;
    adv = Advancement(backwards.size());
    adv.add_encountered();
    adv.update();
    backwards.reset();
    backwards.table[0] = 0;
    compute_pruning_table_backwards(backwards, strat, adv);

    for (unsigned k = 0; k < direct.size(); ++k) {
        assert(backwards.table[k] == direct.table[k]);
    }
    direct.write();
}

template <unsigned nc, unsigned ne>
void test_split_api(const Block<nc, ne>& b) {
    Block<nc, 0> c_sub_block(b.name + "_corners", b.corners, {});
    Block<0, ne> e_sub_block(b.name + "_edges", {}, b.edges);
    Strategy::Optimal c_strat(c_sub_block);
    Strategy::Optimal e_strat(e_sub_block);

    SplitPruningTable table(c_strat, e_strat);
    assert(table.size() > 0);
    table.reset();

    CoordinateBlockCube cbc{0, 0, 0, 0, 0, 0};
    assert(table.get_estimate(cbc) > 0);

    auto strat = Strategy::Split(c_strat, e_strat);
    table = strat.gen_table();
    assert(table.get_estimate(cbc) == 0);
    table.write();
    table.reset();
    assert(table.get_estimate(cbc) != 0);
    table.load();
    assert(table.get_estimate(cbc) == 0);
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
    test_permutation_is_correct(DL_223);

    std::cout << "\nDirect and backwards equivalence test" << std::endl;
    test_direct_and_backward_are_equivalent(LF_column);
    test_direct_and_backward_are_equivalent(DLB_222);

    std::cout << "\nSplit pruning table test" << std::endl;
    test_split_api(DLB_222);
    return 0;
}