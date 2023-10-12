#pragma once
#include <algorithm>  // std::fill(begin, end)
#include <cstdint>    // uint8_t
#include <deque>      // std::deque
#include <limits>     // std::numeric_limits
#include <memory>     // std::unique_ptr

#include "coordinate.hpp"
#include "move_table.hpp"
#include "node.hpp"

namespace fs = std::filesystem;

template <bool verbose = false>
struct Advancement {
    unsigned generated{0};
    unsigned encountered{0};
    unsigned depth{0};
    unsigned table_size;
    std::vector<unsigned> nodes_per_depth{0};

    Advancement(const unsigned table_size) : table_size(table_size){};

    void add_generated() { ++generated; }
    void add_encountered() {
        ++encountered;
        ++nodes_per_depth.back();
    }

    unsigned percent() const {
        return (double(encountered) * 100 / (double)table_size);
    }

    void update() {
        std::cout << "Depth: " << depth << " " << nodes_per_depth.back()
                  << std::endl;
        show();
        ++depth;
        nodes_per_depth.push_back(0);
    }

    void update(const unsigned child_depth) {
        if (child_depth > depth) {
            std::cout << "Depth: " << depth << " " << nodes_per_depth.back()
                      << std::endl;
            show();
            depth = child_depth;
            nodes_per_depth.push_back(0);
        }
    }

    bool stop_breadth_first_generator() const { return percent() > 100.0; }

    void show() {
        if constexpr (verbose) {
            std::cout << percent() << "% "
                      << "    encounter ratio: "
                      << (double)encountered / (double)generated << std::endl;
        }
    }
};

template <typename PruningTable, bool verbose = false>
void compute_pruning_table_backwards(PruningTable& p_table,
                                     Advancement<verbose>& advancement) {
    BlockMoveTable m_table(p_table.b);
    using StorageNode = CompressedNode<unsigned>;
    using WorkNode = Node<CoordinateBlockCube>;
    auto apply = [&m_table](const Move& move, CoordinateBlockCube& CBC) {
        m_table.apply(move, CBC);
    };

    while (advancement.encountered < p_table.table_size) {
        for (unsigned k = 0; k < p_table.size(); ++k) {
            if (p_table.table[k] == PruningTable::unassigned) {
                auto node =
                    WorkNode(p_table.from_index(k), PruningTable::unassigned);
                auto children = node.expand(apply, HTM_Moves);
                for (auto&& child : children) {
                    advancement.add_generated();
                    auto table_entry = p_table.index(child.state);
                    auto child_depth = p_table.table[table_entry];
                    if (child_depth == advancement.depth - 1) {
                        advancement.add_encountered();
                        p_table.table[k] = child_depth + 1;
                        break;
                    }
                }
            }
        }
        advancement.update();
        assert(advancement.depth < 21);
    }
}

template <typename PruningTable, bool verbose>
void compute_pruning_table(PruningTable& p_table,
                           Advancement<verbose>& advancement) {
    using StorageNode = CompressedNode<unsigned>;
    using WorkNode = Node<CoordinateBlockCube>;
    assert(p_table.size() == advancement.table_size);
    BlockMoveTable m_table(p_table.b);
    auto apply = [&m_table](const Move& move, CoordinateBlockCube& CBC) {
        m_table.apply(move, CBC);
    };
    auto compress = [&p_table](const WorkNode& big) {
        return StorageNode(p_table.index(big.state), big.depth);
    };
    auto uncompress = [&p_table](const StorageNode& archived) {
        return WorkNode(p_table.from_index(archived.state), archived.depth);
    };
    p_table.reset();
    StorageNode root;
    auto queue = std::deque{root};
    advancement.add_encountered();
    unsigned table_entry{0};
    assert(table_entry == root.state);
    p_table.table[table_entry] = 0;

    while (queue.size() > 0 && !advancement.stop_breadth_first_generator()) {
        WorkNode node = uncompress(queue.back());
        assert(p_table.index(node.state) < p_table.size());
        assert(p_table.table[p_table.index(node.state)] !=
               PruningTable::unassigned);
        // -1 template parameter deactivates the move
        // sequence copy at each move application
        auto children = node.expand<-1>(apply, HTM_Moves);
        for (auto&& child : children) {
            advancement.add_generated();
            table_entry = p_table.index(child.state);
            if (p_table.table[table_entry] == PruningTable::unassigned) {
                queue.push_front(compress(child));
                p_table.table[table_entry] = child.depth;
                advancement.update(child.depth);
                advancement.add_encountered();
            }
        }
        queue.pop_back();
    }
    advancement.update(advancement.depth + 1);
    // if constexpr (verbose) {
    //     std::cout << "Switching to backwards search" << std::endl;
    // }
    // compute_pruning_table_backwards(p_table, advancement);
    assert(advancement.encountered == p_table.size());
}

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
        table_path = table_dir / b.name;
        if (fs::exists(table_path / filename)) {
            load();
        }
    }

    template <bool verbose = false>
    void gen() const {
        std::cout << "Generating OptimalPruningTable for Block: " << std::endl;
        b.show();
        reset();
        table[0] = 0;
        Advancement<verbose> adv(table_size);
        compute_pruning_table(*this, adv);
        compute_pruning_table_backwards(*this, adv);
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
        table_path = table_dir / b.name;
        if (fs::exists(table_path / filename)) {
            load();
        }
    }

    template <bool verbose = false>
    void gen() const {
        std::cout << "Generating PermutationPruningTable for Block:"
                  << std::endl;
        b.show();
        reset();
        table[0] = 0;
        Advancement<verbose> adv(table_size);
        compute_pruning_table(*this, adv);
        compute_pruning_table_backwards(*this, adv);
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
        std::fill(table.get(), table.get() + size(), unassigned);
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
