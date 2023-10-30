#pragma once
#include <algorithm>  // std::fill(begin, end)
#include <cstdint>    // uint8_t
#include <deque>      // std::deque
#include <limits>     // std::numeric_limits
#include <memory>     // std::unique_ptr

#include "coordinate.hpp"
#include "generator.hpp"
#include "node.hpp"

namespace fs = std::filesystem;

struct LoadError {
    fs::path table_path;
};

template <typename Strategy>
struct PruningTable {
    using entry_type = uint8_t;  // Cannot be printed, max representable
                                 // value of uint8_t is 255
    static constexpr unsigned unassigned =
        std::numeric_limits<entry_type>::max();
    fs::path table_dir = fs::current_path() / "pruning_tables" / Strategy::name;
    fs::path table_path;
    std::string filename = "table.dat";

    std::unique_ptr<entry_type[]> table{new entry_type[Strategy::table_size]};

    PruningTable(){};
    PruningTable(const std::string block_id) {
        table_path = table_dir / block_id;
    };
    template <typename Block>
    PruningTable(const Block& block) {
        table_path = table_dir / block.id;
    };
    PruningTable(const Strategy& strat) {
        table_path = table_dir / strat.block.id;
    }

    void write() const {
        fs::create_directories(table_path);
        {
            std::ofstream file(table_path / filename, std::ios::binary);
            file.write(reinterpret_cast<char*>(table.get()),
                       sizeof(entry_type) * size());
            file.close();
        }
    }

    void load() const {
        if (fs::exists(table_path / filename)) {
            std::ifstream istrm(table_path / filename, std::ios::binary);
            istrm.read(reinterpret_cast<char*>(table.get()),
                       sizeof(entry_type) * size());
            istrm.close();
        } else {
            std::cout << "Pruning table could not be loaded" << std::endl;
            throw LoadError{table_path / filename};
        }
    }

    void reset() const {
        std::fill(table.get(), table.get() + size(), unassigned);
    }

    entry_type& operator[](const unsigned k) const {
        assert(k < size());
        return table[k];
    }

    unsigned get_estimate(const CoordinateBlockCube& cbc) const {
        return table[Strategy::index(cbc)];
    }

    unsigned size() const { return Strategy::table_size; }
};

template <typename CornerStrategy, typename EdgeStrategy>
struct SplitPruningTable {
    // using entry_type = uint8_t;  // Cannot be printed, max representable
    //                              // value of uint8_t is 255
    // static constexpr unsigned unassigned =
    //     std::numeric_limits<entry_type>::max();
    // fs::path table_dir = fs::current_path() / "pruning_tables" /
    // Strategy::name; fs::path table_path; std::string filename = "table.dat";

    PruningTable<CornerStrategy> c_table;
    PruningTable<EdgeStrategy> e_table;

    // SplitPruningTable(){};
    SplitPruningTable(const CornerStrategy& c_strat,
                      const EdgeStrategy& e_strat)
        : c_table{c_strat}, e_table{e_strat} {
        assert(CornerStrategy::n_edge_states == 1);
        assert(EdgeStrategy::n_corner_states == 1);
    }

    void write() const {
        c_table.write();
        e_table.write();
    }

    void load() const {
        c_table.load();
        e_table.load();
    }

    void reset() const {
        c_table.reset();
        e_table.reset();
    }

    unsigned get_estimate(const CoordinateBlockCube& cbc) const {
        auto edge_estimate = e_table.get_estimate(cbc);
        auto corner_estimate = c_table.get_estimate(cbc);
        return (corner_estimate < edge_estimate) ? edge_estimate
                                                 : corner_estimate;
    }

    unsigned size() const {
        return CornerStrategy::table_size + EdgeStrategy::table_size;
    }
};

struct NullPruningTable {
    template <typename Cube>
    unsigned get_estimate(const Cube& cube) const {
        if (cube.is_solved()) {
            return 0;
        } else {
            return 1;
        }
    }
};

namespace Strategy {
template <unsigned nc, unsigned ne>
struct Optimal {
    static constexpr unsigned n_cp = factorial(nc);
    static constexpr unsigned n_co = ipow(3, nc);
    static constexpr unsigned n_ep = factorial(ne);
    static constexpr unsigned n_eo = ipow(2, ne);
    static constexpr unsigned n_corner_states =
        (factorial(8) / factorial(8 - nc)) * ipow(3, nc);
    static constexpr unsigned n_edge_states =
        (factorial(12) / factorial(12 - ne)) * ipow(2, ne);
    static constexpr unsigned table_size = n_corner_states * n_edge_states;
    static constexpr std::string_view name = "optimal";
    Block<nc, ne> block;

    Optimal() {}
    Optimal(const Block<nc, ne>& b) : block{b} {}

    template <bool verbose = false>
    PruningTable<Optimal<nc, ne>> gen_table() const {
        PruningTable<Optimal<nc, ne>> table(block.id);
        generate<verbose>(table, *this);
        return table;
    }

    PruningTable<Optimal<nc, ne>> load_table() const {
        PruningTable<Optimal<nc, ne>> table(block.id);
        table.load();
        return table;
    }

    static unsigned c_index(const CoordinateBlockCube& cbc) {
        unsigned index = cbc.ccl * n_cp * n_co + (cbc.ccp * n_co + cbc.cco);
        return index;
    }

    static unsigned e_index(const CoordinateBlockCube& cbc) {
        unsigned index = cbc.cel * n_ep * n_eo + (cbc.cep * n_eo + cbc.ceo);
        return index;
    }

    static unsigned index(const CoordinateBlockCube& cbc) {
        auto index = e_index(cbc) * n_corner_states + c_index(cbc);
        assert(index < table_size);
        return index;
    }

    static CoordinateBlockCube from_index(const uint& index) {
        return CoordinateBlockCube(
            (index % n_corner_states) / (n_cp * n_co),           // ccl
            (index / n_corner_states) / (n_ep * n_eo),           // cel
            ((index % n_corner_states) % (n_cp * n_co)) / n_co,  // ccp
            ((index / n_corner_states) % (n_ep * n_eo)) / n_eo,  // cep
            ((index % n_corner_states) % (n_cp * n_co)) % n_co,  // cco
            ((index / n_corner_states) % (n_ep * n_eo)) % n_eo   // ceo
        );
    }

    void show() const {
        std::cout << "Strategy Object:" << std::endl;
        std::cout << "   Type: " << name << std::endl;
        block.show();
    }
};

template <unsigned nc, unsigned ne>
struct Permutation {
    static constexpr unsigned n_cp = factorial(nc);
    static constexpr unsigned n_ep = factorial(ne);
    static constexpr unsigned n_corner_states =
        (factorial(NC) / factorial(NC - nc));
    static constexpr unsigned n_edge_states =
        (factorial(NE) / factorial(NE - ne));
    static constexpr unsigned table_size = n_corner_states * n_edge_states;
    static constexpr std::string_view name = "permutation";
    const Block<nc, ne> block;

    Permutation() {}
    Permutation(const Block<nc, ne>& b) : block{b} {}

    template <bool verbose = false>
    PruningTable<Permutation<nc, ne>> gen_table() {
        PruningTable<Permutation<nc, ne>> table(block.id);
        generate<verbose>(table, *this);
        return table;
    }

    PruningTable<Permutation<nc, ne>> load_table() const {
        PruningTable<Permutation<nc, ne>> table(block.id);
        table.load();
        return table;
    }

    static unsigned c_index(const CoordinateBlockCube& cbc) {
        unsigned index = cbc.ccl * n_cp + cbc.ccp;
        return index;
    }

    static unsigned e_index(const CoordinateBlockCube& cbc) {
        unsigned index = cbc.cel * n_ep + cbc.cep;
        return index;
    }

    static unsigned index(const CoordinateBlockCube& cbc) {
        auto index = e_index(cbc) * n_corner_states + c_index(cbc);
        assert(index < table_size);
        return index;
    }

    static CoordinateBlockCube from_index(const unsigned& index) {
        return CoordinateBlockCube((index % n_corner_states) / n_cp,  // ccl
                                   (index / n_corner_states) / n_ep,  // cel
                                   (index % n_corner_states) % n_cp,  // ccp
                                   (index / n_corner_states) % n_ep,  // cep
                                   0,                                 // cco
                                   0                                  // ceo
        );
    }
    void show() const {
        std::cout << "Strategy Object:" << std::endl;
        std::cout << "   Type: " << name << std::endl;
        block.show();
    }
};

template <typename CornerStrategy, typename EdgeStrategy>
struct Split {
    CornerStrategy c_strat;
    EdgeStrategy e_strat;

    Split(const CornerStrategy& cs, const EdgeStrategy& es)
        : c_strat{cs}, e_strat{es} {}

    template <bool verbose = false>
    SplitPruningTable<CornerStrategy, EdgeStrategy> gen_table() const {
        SplitPruningTable<CornerStrategy, EdgeStrategy> table(c_strat, e_strat);
        table.c_table = c_strat.template gen_table<verbose>();
        table.e_table = e_strat.template gen_table<verbose>();
        return table;
    }

    SplitPruningTable<CornerStrategy, EdgeStrategy> load_table() const {
        SplitPruningTable<CornerStrategy, EdgeStrategy> table(c_strat, e_strat);
        table.load();
        return table;
    }
};
}  // namespace Strategy