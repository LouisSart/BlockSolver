#include <vector>

#include "move_table.hpp"
#include "node.hpp"

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

template <typename PruningTable, bool verbose = false>
void compute_pruning_table_backwards(PruningTable& p_table,
                                     Advancement<verbose>& advancement) {
    BlockMoveTable m_table(p_table.b);
    auto apply = [&m_table](const Move& move, CoordinateBlockCube& CBC) {
        m_table.apply(move, CBC);
    };
    using WorkNode = Node<CoordinateBlockCube>;

    while (advancement.encountered < p_table.table_size) {
        for (unsigned k = 0; k < p_table.size(); ++k) {
            if (p_table[k] == PruningTable::unassigned) {
                auto node =
                    WorkNode(p_table.from_index(k), PruningTable::unassigned);
                auto children = node.expand(apply, HTM_Moves);
                for (auto&& child : children) {
                    advancement.add_generated();
                    auto table_entry = p_table.index(child.state);
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
    p_table[table_entry] = 0;

    while (queue.size() > 0 && !advancement.stop_breadth_first_generator()) {
        WorkNode node = uncompress(queue.back());
        assert(p_table.index(node.state) < p_table.size());
        assert(p_table[p_table.index(node.state)] != PruningTable::unassigned);
        // -1 template parameter deactivates the move
        // sequence copy at each move application
        auto children = node.expand<-1>(apply, HTM_Moves);
        for (auto&& child : children) {
            advancement.add_generated();
            table_entry = p_table.index(child.state);
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