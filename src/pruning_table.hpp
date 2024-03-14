#pragma once
#include <algorithm>   // std::fill(begin, end)
#include <cstdint>     // uint8_t
#include <deque>       // std::deque
#include <filesystem>  // locate table files
#include <fstream>     // write tables into files
#include <limits>      // std::numeric_limits
#include <memory>      // std::unique_ptr

#include "coordinate.hpp"
#include "generator.hpp"
#include "move_table.hpp"
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

    PruningTable() { table_path = table_dir; }
    PruningTable(const std::string id) { table_path = table_dir / id; }

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

    PruningTable<Strategy>* get_ptr() { return this; }

    unsigned size() const { return Strategy::table_size; }
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

    NullPruningTable* get_ptr() { return this; }
};

namespace Strategy {
template <unsigned nc, unsigned ne>
struct Optimal {
    static constexpr unsigned n_cp = factorial(nc);
    using table_t = PruningTable<Optimal<nc, ne>>;
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
        generate<verbose>(table, *this, BlockMoveTable(block));
        return table;
    }

    PruningTable<Optimal<nc, ne>> load_table() const {
        PruningTable<Optimal<nc, ne>> table(block.id);
        table.load();
        return table;
    }

    std::string info() const {
        std::string ret = "Optimal strategy for block " + block.name;
        ret += "\n";
        ret += "Corners: ";
        for (auto c : block.corners) {
            ret += corner_str[c] + " ";
        }
        ret += "\n";
        ret += "Edges: ";
        for (auto c : block.edges) {
            ret += edge_str[c] + " ";
        }
        ret += "\n";
        ret += "Table size: " + std::to_string(table_size) + "\n";
        return ret;
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
    using table_t = PruningTable<Permutation<nc, ne>>;
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
    PruningTable<Permutation<nc, ne>> gen_table() const {
        PruningTable<Permutation<nc, ne>> table(block.id);
        generate<verbose>(table, *this, BlockMoveTable(block));
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

    std::string info() const {
        std::string ret = "Permutation strategy for block " + block.name;
        ret += "\n";
        ret += "Corners: ";
        for (auto c : block.corners) {
            ret += corner_str[c] + " ";
        }
        ret += "\n";
        ret += "Edges: ";
        for (auto c : block.edges) {
            ret += edge_str[c] + " ";
        }
        ret += "\n";
        ret += "Table size: " + std::to_string(table_size) + "\n";
        return ret;
    }

    void show() const {
        std::cout << "Strategy Object:" << std::endl;
        std::cout << "   Type: " << name << std::endl;
        block.show();
    }
};

struct OptimalEO {
    using table_type = PruningTable<OptimalEO>;
    static constexpr unsigned table_size = ipow(2, NE - 1);
    static constexpr std::string_view name = "eo";

    static unsigned index(const CoordinateBlockCube& cbc) {
        auto index = cbc.ceo;
        assert(index < table_size);
        return index;
    }

    static CoordinateBlockCube from_index(const unsigned& index) {
        return CoordinateBlockCube(0, 0, 0, 0, 0, index);
    }

    std::string info() const {
        std::string ret = "Optimal strategy for full EO:\n";
        ret += "Table size: " + std::to_string(table_size);
        return ret;
    }

    template <bool verbose = false>
    table_type gen_table() const {
        table_type table;
        generate<verbose>(table, *this, EOMoveTable());
        return table;
    }

    table_type load_table() const {
        table_type table;
        table.load();
        return table;
    }
};
}  // namespace Strategy

template <bool verbose = false, typename Strategy>
Strategy::table_t* gen_table_ptr(const Strategy& strat) {
    auto table = new Strategy::table_t(strat.block.id);
    *table = strat.template gen_table<verbose>();
    return table;
}

template <typename Strategy>
Strategy::table_t* load_table_ptr(const Strategy& strat) {
    auto table = new Strategy::table_t(strat.block.id);
    try {
        *table = strat.template load_table();
    } catch (LoadError error) {
        std::cout << "Do you want to generate pruning table ? (y/n) "
                  << std::endl;
        std::string answer;
        std::cin >> answer;
        if (answer == "y") {
            *table = strat.template gen_table();
        } else {
            abort();
        };
    }
    return table;
}