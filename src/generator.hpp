#include <cassert>
#include <vector>

#include "algorithm.hpp"
#include "coordinate_block_cube.hpp"

template <bool verbose = false>
struct Advancement {
    unsigned generated{0};
    unsigned encountered{0};
    unsigned depth{0};
    unsigned table_size;
    float encounter_ratio_limit{0.0};
    float percent_limit{100.0};
    std::vector<unsigned> nodes_per_depth{0};

    Advancement(const unsigned table_size) : table_size(table_size){};
    Advancement(const unsigned table_size, const float percent_switch,
                const float encounter_ratio_switch)
        : table_size(table_size),
          percent_limit(percent_switch),
          encounter_ratio_limit(encounter_ratio_switch){};

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

    bool stop_breadth_first_generator() const {
        return (percent() > percent_limit ||
                encounter_ratio() < encounter_ratio_limit);
    }

    float encounter_ratio() const {
        return (double)encountered / (double)generated;
    }

    void show() {
        if constexpr (verbose) {
            std::cout << percent() << "% "
                      << "    encounter ratio: " << encounter_ratio()
                      << std::endl;
        }
    }
};

// Basic Nodes for pruning table generation
struct GenNode {
    CoordinateBlockCube state;
    uint8_t depth;

    template <typename MTable>
    auto expand(const MTable& mt) {
        std::vector<GenNode> ret;
        CoordinateBlockCube next;
        for (const Move move : HTM_Moves) {
            next = state;
            mt.apply(move, next);
            ret.emplace_back(next, depth + 1);
        }
        return ret;
    }
};

struct CompressedNode {
    unsigned state;  // single-digit representation of a cube state
    uint8_t depth;
};

template <bool verbose, typename PruningTable, typename Strategy,
          typename MoveTable>
void compute_pruning_table_backwards(PruningTable& p_table, Strategy& strat,
                                     const MoveTable& m_table,
                                     Advancement<verbose>& advancement) {
    assert(p_table.size() == strat.table_size);
    assert(p_table.size() == advancement.table_size);

    while (advancement.encountered < p_table.size()) {
        for (unsigned k = 0; k < p_table.size(); ++k) {
            if (p_table[k] == PruningTable::unassigned) {
                auto node =
                    GenNode{strat.from_index(k), PruningTable::unassigned};
                auto children = node.expand(m_table);
                for (auto&& child : children) {
                    advancement.add_generated();
                    auto table_entry = strat.index(child.state);
                    auto child_depth = p_table[table_entry];
                    if (child_depth == advancement.depth - 1) {
                        advancement.add_encountered();
                        p_table[k] = child_depth + 1;
                        break;
                    }
                }
            }
        }
        advancement.update();
        assert(advancement.depth < 21);
    }
}

template <bool verbose, typename PruningTable, typename Strategy,
          typename MoveTable>
void compute_pruning_table(PruningTable& p_table, Strategy& strat,
                           const MoveTable& m_table,
                           Advancement<verbose>& advancement) {
    assert(p_table.size() == advancement.table_size);
    assert(p_table.size() == strat.table_size);
    auto compress = [](const GenNode& big) {
        return CompressedNode{Strategy::index(big.state), big.depth};
    };
    auto uncompress = [](const CompressedNode& archived) {
        return GenNode{Strategy::from_index(archived.state), archived.depth};
    };
    p_table.reset();
    CompressedNode root{0, 0};
    auto queue = std::deque{root};
    advancement.add_encountered();
    unsigned table_entry{0};
    assert(table_entry == root.state);
    p_table[table_entry] = 0;

    while (queue.size() > 0 && !advancement.stop_breadth_first_generator()) {
        auto node = uncompress(queue.back());
        assert(strat.index(node.state) < p_table.size());
        assert(p_table[strat.index(node.state)] != PruningTable::unassigned);
        auto children = node.expand(m_table);
        for (auto&& child : children) {
            advancement.add_generated();
            table_entry = strat.index(child.state);
            if (p_table[table_entry] == PruningTable::unassigned) {
                queue.push_front(compress(child));
                p_table[table_entry] = child.depth;
                advancement.update(child.depth);
                advancement.add_encountered();
            }
        }
        queue.pop_back();
    }
}

template <bool verbose = false, typename PruningTable, typename Strategy,
          typename MoveTable>
void generate(PruningTable& p_table, const Strategy& strat,
              const MoveTable& m_table) {
    p_table.reset();
    p_table[0] = 0;
    const float percent_switch = 70.0;
    const float encounter_ratio_switch = 0.3;
    Advancement<verbose> adv(p_table.size(), percent_switch,
                             encounter_ratio_switch);
    compute_pruning_table(p_table, strat, m_table, adv);
    if constexpr (verbose) {
        std::cout << "Switching to backwards search" << std::endl;
    }
    adv.show();
    compute_pruning_table_backwards(p_table, strat, m_table, adv);
    assert(adv.encountered == p_table.size());
    p_table.write();

    {
        std::ofstream file(p_table.table_path / "info.txt", std::ios::out);
        file << strat.info() << std::endl;
        file << "Depth Nodes" << std::endl;
        int k = 0;
        for (auto n : adv.nodes_per_depth) {
            file << k << " " << n << std::endl;
            k++;
        }
        file.close();
    }
}
