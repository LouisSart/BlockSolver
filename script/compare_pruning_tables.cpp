#include "pruning_table.hpp"

auto DL_223 = Block<2, 5>("DL_223", {DLF, DLB}, {LF, LB, DF, DB, DL});

template <typename Strategy>
double mean_value(const Strategy& strat) {
    auto table = strat.load_table();
    long unsigned sum = 0;
    for (unsigned k = 0; k < table.size(); ++k) {
        sum += (unsigned)table.table[k];
    }
    double mean = (double)sum / table.size();
    return mean;
}

int main() {
    std::cout << mean_value(Strategy::Permutation(DL_223)) << std::endl;
    std::cout << mean_value(Strategy::Optimal(DL_223)) << std::endl;
}