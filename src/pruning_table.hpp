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

template <unsigned nc, unsigned ne>
struct OptimalPruningTable {
    using entry_type = uint8_t;  // Cannot be printed, max representable
                                 // value of uint8_t is 255

    static constexpr unsigned unassigned =
        std::numeric_limits<entry_type>::max();
    static constexpr unsigned n_cp = factorial(nc);
    static constexpr unsigned n_co = ipow(3, nc);
    static constexpr unsigned n_ep = factorial(ne);
    static constexpr unsigned n_eo = ipow(2, ne);
    static constexpr unsigned n_corner_states =
        (factorial(8) / factorial(8 - nc)) * ipow(3, nc);
    static constexpr unsigned n_edge_states =
        (factorial(12) / factorial(12 - ne)) * ipow(2, ne);
    static constexpr unsigned table_size = n_corner_states * n_edge_states;
    std::unique_ptr<entry_type[]> table{new entry_type[table_size]};

    fs::path table_dir = fs::current_path() / "pruning_tables/optimal/";
    fs::path table_path;
    std::string filename = "table.dat";

    Block<nc, ne> b;

    OptimalPruningTable(){};
    OptimalPruningTable(const Block<nc, ne>& block) : b{block} {
        table_path = table_dir / b.id;
        if (fs::exists(table_path / filename)) {
            load();
        }
    }

    template <bool verbose = false>
    void gen() const {
        reset();
        table[0] = 0;
        const float percent_switch = 70.0;
        const float encounter_ratio_switch = 0.3;
        Advancement<verbose> adv(table_size, percent_switch,
                                 encounter_ratio_switch);
        compute_pruning_table(*this, adv);
        if constexpr (verbose) {
            std::cout << "Switching to backwards search" << std::endl;
        }
        adv.show();
        compute_pruning_table_backwards(*this, adv);
        assert(adv.encountered == table_size);
        write();
    }

    void write() const {
        fs::create_directories(table_path);
        {
            std::ofstream file(table_path / filename, std::ios::binary);
            file.write(reinterpret_cast<char*>(table.get()),
                       sizeof(entry_type) * table_size);
            file.close();
        }
    }

    void load() const {
        assert(fs::exists(table_path));
        std::ifstream istrm(table_path / filename, std::ios::binary);
        istrm.read(reinterpret_cast<char*>(table.get()),
                   sizeof(entry_type) * table_size);
        istrm.close();
    }

    void reset() const {
        std::fill(table.get(), table.get() + size(), unassigned);
    }

    uint c_index(const CoordinateBlockCube& cbc) const {
        uint index = cbc.ccl * n_cp * n_co + (cbc.ccp * n_co + cbc.cco);
        return index;
    }

    uint e_index(const CoordinateBlockCube& cbc) const {
        uint index = cbc.cel * n_ep * n_eo + (cbc.cep * n_eo + cbc.ceo);
        return index;
    }

    uint index(const CoordinateBlockCube& cbc) const {
        auto index = e_index(cbc) * n_corner_states + c_index(cbc);
        assert(index < table_size);
        return index;
    }

    entry_type& operator[](const unsigned k) const {
        assert(k < table_size);
        return table[k];
    }

    CoordinateBlockCube from_index(const uint& index) const {
        return CoordinateBlockCube(
            (index % n_corner_states) / (n_cp * n_co),           // ccl
            (index / n_corner_states) / (n_ep * n_eo),           // cel
            ((index % n_corner_states) % (n_cp * n_co)) / n_co,  // ccp
            ((index / n_corner_states) % (n_ep * n_eo)) / n_eo,  // cep
            ((index % n_corner_states) % (n_cp * n_co)) % n_co,  // cco
            ((index / n_corner_states) % (n_ep * n_eo)) % n_eo   // ceo
        );
    }
    unsigned get_estimate(const CoordinateBlockCube& cbc) const {
        return table[index(cbc)];
    }

    unsigned size() const { return table_size; }
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

template <unsigned nc, unsigned ne>
struct PermutationPruningTable {
    using entry_type = uint8_t;  // Cannot be printed, max representable
    // value of uint8_t is 255

    static constexpr unsigned unassigned =
        std::numeric_limits<entry_type>::max();
    static constexpr uint n_cp = factorial(nc);
    static constexpr uint n_ep = factorial(ne);
    static constexpr uint n_corner_states = (factorial(8) / factorial(8 - nc));
    static constexpr uint n_edge_states = (factorial(12) / factorial(12 - ne));
    static constexpr uint table_size = n_corner_states * n_edge_states;
    std::unique_ptr<entry_type[]> table{new entry_type[table_size]};

    fs::path table_dir = fs::current_path() / "pruning_tables/permutation/";
    fs::path table_path;
    std::string filename = "table.dat";

    const Block<nc, ne> b;

    PermutationPruningTable(){};
    PermutationPruningTable(const Block<nc, ne>& block) : b{block} {
        table_path = table_dir / b.id;
        if (fs::exists(table_path / filename)) {
            load();
        }
    }

    template <bool verbose = false>
    void gen() const {
        reset();
        table[0] = 0;
        const float percent_switch = 70.0;
        const float encounter_ratio_switch = 0.3;
        Advancement<verbose> adv(table_size, percent_switch,
                                 encounter_ratio_switch);
        compute_pruning_table(*this, adv);
        if constexpr (verbose) {
            std::cout << "Switching to backwards search" << std::endl;
        }
        adv.show();
        compute_pruning_table_backwards(*this, adv);
        assert(adv.encountered == table_size);
        write();
    }

    void write() const {
        fs::create_directories(table_path);
        {
            std::ofstream file(table_path / filename, std::ios::binary);
            file.write(reinterpret_cast<char*>(table.get()),
                       sizeof(entry_type) * table_size);
            file.close();
        }
    }

    void load() {
        std::ifstream istrm(table_path / filename, std::ios::binary);
        istrm.read(reinterpret_cast<char*>(table.get()),
                   sizeof(entry_type) * table_size);
        istrm.close();
    }

    void reset() const {
        std::fill(table.get(), table.get() + table_size, unassigned);
    }

    uint c_index(const CoordinateBlockCube& cbc) const {
        uint index = cbc.ccl * n_cp + cbc.ccp;
        return index;
    }

    uint e_index(const CoordinateBlockCube& cbc) const {
        uint index = cbc.cel * n_ep + cbc.cep;
        return index;
    }

    uint index(const CoordinateBlockCube& cbc) const {
        auto index = e_index(cbc) * n_corner_states + c_index(cbc);
        assert(index < table_size);
        return index;
    }

    entry_type& operator[](const unsigned k) const {
        assert(k < table_size);
        return table[k];
    }

    CoordinateBlockCube from_index(const unsigned& index) const {
        return CoordinateBlockCube((index % n_corner_states) / n_cp,  // ccl
                                   (index / n_corner_states) / n_ep,  // cel
                                   (index % n_corner_states) % n_cp,  // ccp
                                   (index / n_corner_states) % n_ep,  // cep
                                   0,                                 // cco
                                   0                                  // ceo
        );
    }

    unsigned get_estimate(const CoordinateBlockCube& cbc) const {
        return table[index(cbc)];
    }
    unsigned size() const { return table_size; }
};
