#include <array>
#include <deque>
#include <cstdint>
#include <limits>
#include "coordinate.hpp"
#include "move_table.hpp"
#include "node.hpp"

template<uint nc, uint ne>
struct OptimalPruningTable
{
  using entry_type = uint8_t; // Cannot be printed, max representable value of uint8_t is 255
  static constexpr uint n_cp = factorial(nc);
  static constexpr uint n_co = ipow(3, nc);
  static constexpr uint n_ep = factorial(ne);
  static constexpr uint n_eo = ipow(2, ne);
  static constexpr uint n_corner_states = (factorial(8)/factorial(8 - nc))*ipow(3, nc);
  static constexpr uint n_edge_states = (factorial(12)/factorial(12 - ne))*ipow(2, ne);
  static constexpr uint table_size = n_corner_states * n_edge_states;
  std::array<entry_type, table_size> table;

  std::string name;
  std::filesystem::path table_dir_path{};
  std::filesystem::path block_table_path;

  OptimalPruningTable(){};
  OptimalPruningTable(const Block<nc,ne> &b)
  {
    table_dir_path = fs::current_path() / "pruning_tables/";
    block_table_path = table_dir_path / b.name;
    if (std::filesystem::exists(block_table_path))
    {
      this->load();
    }
    else
    {
      std::cout << "Pruning table directory not found, building the tables\n";
      std::filesystem::create_directories(block_table_path);
      compute_table(b);
      this->write();
    }
  }
  void write()
  {
    std::filesystem::path table_file = block_table_path / "table.dat";
    {
      std::ofstream file(table_file, std::ios::binary);
      file.write(reinterpret_cast<char*>(table.data()), sizeof(entry_type)*table_size);
      file.close();
    }
  }

  void load()
  {
    std::filesystem::path table_path = block_table_path / "table.dat";

    std::ifstream istrm(table_path, std::ios::binary);
    istrm.read(reinterpret_cast<char*>(table.data()), sizeof(uint)*table_size);
    istrm.close();
  }

  uint c_index(const CoordinateBlockCube &cbc) const {
    uint index = cbc.ccl * n_cp * n_co + (cbc.ccp * n_co + cbc.cco);
    return index;
  }

  uint e_index(const CoordinateBlockCube &cbc) const {
    uint index = cbc.cel * n_ep * n_eo + (cbc.cep * n_eo + cbc.ceo);
    return index;
  }

  uint index(const CoordinateBlockCube &cbc) const {
    auto index = e_index(cbc) * n_corner_states + c_index(cbc);
    assert(index < table_size);
    return index;
  }

  uint get_estimate(const CoordinateBlockCube &cbc) const {
    return table[index(cbc)];
  }

  void compute_table(const Block<nc,ne> &b){
    BlockMoveTable m_table(b);
    auto apply = [m_table](const Move& move, CoordinateBlockCube& CBC){m_table.apply(move,CBC);};

    auto unassigned = std::numeric_limits<entry_type>::max();
    table.fill(unassigned);
    Node<CoordinateBlockCube> root, node;
    auto queue = std::deque{root};
    uint table_entry{0};
    std::vector<uint> state_counter{0};
    assert(table_entry == index(root.state));
    table[table_entry] = 0;

    while (queue.size() > 0){
      node = queue.back();
      assert(index(node.state) < table_size);
      assert(table[index(node.state)] != unassigned);
      auto children = node.expand<-1>( // -1 template parameter deactivates the move sequence copy at each move application
          apply, allowed_next(node.sequence.back())
      );
      for (auto&& child : children){
        table_entry = index(child.state);
        if (table[table_entry] == unassigned){
          queue.push_front(child);
          table[table_entry] = child.depth;
        }
      }
      if (node.depth == state_counter.size() - 1){
        ++state_counter.back();
      }
      else {
        std::cout << "Depth: " << state_counter.size() - 1 << " " << state_counter.back() << std::endl;
        state_counter.push_back(1);
      }

      queue.pop_back();
    }
    std::cout << "Depth: " << state_counter.size() - 1 << " " << state_counter.back() << std::endl;
    assert(table[0] == 0);
    for (auto&& k : table) {
      assert(k != unassigned);
    }
  }

};

struct NullPruningTable {
  template<typename Cube>
  uint get_estimate(const Cube& cube) const {
    return 0;
  }
};