#pragma once
#include <cstdint>
#include <deque>
#include <limits>
#include <memory>  // std::unique_ptr

#include "coordinate.hpp"
#include "move_table.hpp"
#include "node.hpp"

template <uint nc, uint ne>
struct OptimalPruningTable {
    using entry_type = uint8_t;  // Cannot be printed, max representable value
                                 // of uint8_t is 255
    static constexpr uint n_cp = factorial(nc);
    static constexpr uint n_co = ipow(3, nc);
    static constexpr uint n_ep = factorial(ne);
    static constexpr uint n_eo = ipow(2, ne);
    static constexpr uint n_corner_states =
        (factorial(8) / factorial(8 - nc)) * ipow(3, nc);
    static constexpr uint n_edge_states =
        (factorial(12) / factorial(12 - ne)) * ipow(2, ne);
    static constexpr uint table_size = n_corner_states * n_edge_states;
    std::unique_ptr<entry_type[]> table{new entry_type[table_size]};

    std::string name;
    std::filesystem::path table_dir_path{};
    std::filesystem::path block_table_path;

    OptimalPruningTable(){};
    OptimalPruningTable(const Block<nc, ne>& b) {
        table_dir_path = fs::current_path() / "pruning_tables/";
        block_table_path = table_dir_path / b.name;
        if (std::filesystem::exists(block_table_path)) {
            this->load();
        } else {
            std::cout
                << "Pruning table directory not found, building the tables\n";
            std::filesystem::create_directories(block_table_path);
            compute_table(b);
            this->write();
        }
    }
    void write() {
        std::filesystem::path table_file = block_table_path / "table.dat";
        {
            std::ofstream file(table_file, std::ios::binary);
            file.write(reinterpret_cast<char*>(table.get()),
                       sizeof(entry_type) * table_size);
            file.close();
        }
    }

    void load() {
        std::filesystem::path table_path = block_table_path / "table.dat";

        std::ifstream istrm(table_path, std::ios::binary);
        istrm.read(reinterpret_cast<char*>(table.get()),
                   sizeof(entry_type) * table_size);
        istrm.close();
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

    uint get_estimate(const CoordinateBlockCube& cbc) const {
        return table[index(cbc)];
    }

    void compute_table(const Block<nc, ne>& b) {
        using StorageNode = CompressedNode<uint>;
        using WorkNode = Node<CoordinateBlockCube>;
        BlockMoveTable m_table(b);
        auto apply = [&m_table](const Move& move, CoordinateBlockCube& CBC) {
            m_table.apply(move, CBC);
        };
        auto compress = [this](const WorkNode& big) {
            return StorageNode(this->index(big.state), big.depth);
        };
        auto uncompress = [this](const StorageNode& archived) {
            return WorkNode(this->from_index(archived.state), archived.depth);
        };

        auto unassigned = std::numeric_limits<entry_type>::max();
        this->fill(unassigned);
        StorageNode root;
        WorkNode node;
        auto queue = std::deque{root};
        uint table_entry{0};
        std::vector<uint> state_counter{0};
        // assert(table_entry == index(root.state));
        table[table_entry] = 0;

        while (queue.size() > 0) {
            node = uncompress(queue.back());
            assert(index(node.state) < table_size);
            assert(table[index(node.state)] != unassigned);
            auto children =
                node.expand<-1>(  // -1 template parameter deactivates the move
                                  // sequence copy at each move application
                    apply, allowed_next(node.sequence.back()));
            for (auto&& child : children) {
                table_entry = index(child.state);
                if (table[table_entry] == unassigned) {
                    queue.push_front(compress(child));
                    table[table_entry] = child.depth;
                }
            }
            if (node.depth == state_counter.size() - 1) {
                ++state_counter.back();
            } else {
                std::cout << "Depth: " << state_counter.size() - 1 << " "
                          << state_counter.back() << std::endl;
                state_counter.push_back(1);
            }

            queue.pop_back();
        }
        std::cout << "Depth: " << state_counter.size() - 1 << " "
                  << state_counter.back() << std::endl;
        assert(table[0] == 0);
        int n_states = 0;
        for (auto&& k : state_counter) {
            n_states += k;
        }
        assert(n_states == table_size);
    }

    void fill(const entry_type value) const {
        for (int i = 0; i < table_size; ++i) {
            table[i] = value;
        }
    }
};

struct NullPruningTable {
    template <typename Cube>
    uint get_estimate(const Cube& cube) const {
        if (cube.is_solved()) {
            return 0;
        } else {
            return 1;
        }
    }
};