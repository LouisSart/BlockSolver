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
    fs::path table_dir = fs::current_path() / "pruning_tables";
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
        assert(file_exists());
        std::ifstream istrm(table_path / filename, std::ios::binary);
        istrm.read(reinterpret_cast<char*>(table.get()),
                   sizeof(entry_type) * size());
        istrm.close();
    }

    bool file_exists() const { return (fs::exists(table_path / filename)); }

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
    Block<nc, ne> block;

    Optimal() {}
    Optimal(const Block<nc, ne>& b) : block{b} {}

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

    std::string id() const { return block.id; }

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

    unsigned get_index_of_solved() {
        return index(block.to_coordinate_block_cube(CubieCube()));
    }
};

struct OptimalEO {
    static constexpr unsigned table_size = ipow(2, NE - 1);

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
    std::string id() const { return "eo"; }

    unsigned get_index_of_solved() { return 0; }
};
}  // namespace Strategy

template <unsigned nc, unsigned ne, bool verbose = false>
PruningTable<Strategy::Optimal<nc, ne>> generate_pruning_table(
    Block<nc, ne>& b) {
    Strategy::Optimal<nc, ne> strat(b);
    PruningTable<Strategy::Optimal<nc, ne>> table(b.id);
    generate<verbose>(table, strat, BlockMoveTable(b));
    return table;
}

template <unsigned nc, unsigned ne>
PruningTable<Strategy::Optimal<nc, ne>> load_pruning_table(Block<nc, ne>& b) {
    PruningTable<Strategy::Optimal<nc, ne>> table(b.id);
    if (table.file_exists()) {
        table.load();
    } else {
        std::cout << "Pruning table not found, generating" << std::endl;
        table = generate_pruning_table(b);
        table.write();
    }
    return table;
}

template <bool verbose = false>
PruningTable<Strategy::OptimalEO> generate_eo_table() {
    Strategy::OptimalEO strat;
    PruningTable<Strategy::OptimalEO> table("eo");
    generate<verbose>(table, strat, EOMoveTable());
    return table;
}

PruningTable<Strategy::OptimalEO> load_eo_table() {
    PruningTable<Strategy::OptimalEO> table("eo");
    if (table.file_exists()) {
        table.load();
    } else {
        std::cout << "Pruning table not found, generating" << std::endl;
        table = generate_eo_table();
        table.write();
    }
    return table;
}