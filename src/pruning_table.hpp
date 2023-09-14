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
  static constexpr uint c_table_size = (factorial(8)/factorial(8 - nc))*ipow(3, nc);
  std::array<entry_type, c_table_size> c_table;
  static constexpr uint e_table_size = (factorial(12)/factorial(12 - ne))*ipow(2, ne);
  std::array<entry_type, e_table_size> e_table;

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
      compute_corner_pruning_table(b);
      compute_edge_pruning_table(b);
      this->write();
    }
  }
  void write()
  {
    std::filesystem::path c_table_file = block_table_path / "c_table.dat";
    std::filesystem::path e_table_file = block_table_path / "e_table.dat";
    {
      std::ofstream file(c_table_file, std::ios::binary);
      file.write(reinterpret_cast<char*>(c_table.data()), sizeof(entry_type)*c_table_size);
      file.close();
      file.open(e_table_file, std::ios::binary);
      file.write(reinterpret_cast<char*>(e_table.data()), sizeof(entry_type)*e_table_size);
      file.close();
    }
  }

  void load()
  {
    std::filesystem::path c_table_path = block_table_path / "c_table.dat";
    std::filesystem::path e_table_path = block_table_path / "e_table.dat";

    std::ifstream istrm(c_table_path, std::ios::binary);
    istrm.read(reinterpret_cast<char*>(c_table.data()), sizeof(uint)*c_table_size);
    istrm.close();
    istrm.open(e_table_path, std::ios::binary);
    istrm.read(reinterpret_cast<char*>(e_table.data()), sizeof(uint)*e_table_size);
    istrm.close();
  }

  uint index(const CoordinateBlockCube &cbc){
    
    uint n_cp = factorial(nc);
    uint n_co = ipow(3, nc);
    uint index = cbc.ccl * n_cp * n_co + (cbc.ccp * n_co + cbc.cco);
    assert(index < c_table_size);
    return index;
  }

  uint e_index(const CoordinateBlockCube &cbc){
    
    uint n_ep = factorial(ne);
    uint n_eo = ipow(2, ne);
    uint index = cbc.cel * n_ep * n_eo + (cbc.cep * n_eo + cbc.ceo);
    assert(index < e_table_size);
    return index;
  }

  void compute_corner_pruning_table(const Block<nc,ne> &b){
    BlockMoveTable m_table(b);
    auto apply = [m_table](const Move& move, CoordinateBlockCube& CBC){m_table.apply(move,CBC);};

    auto unassigned = std::numeric_limits<entry_type>::max();
    c_table.fill(unassigned);
    auto queue = std::deque{Node<CoordinateBlockCube>()};
    uint fill_counter = 0;

    while (fill_counter < c_table_size && queue.size() > 0){
      auto node = queue.back();
      auto table_entry = index(node.state);
      assert(table_entry < c_table_size);
      if (c_table[table_entry] == unassigned){
        c_table[table_entry] = node.depth;
        auto children = node.expand(
            apply, allowed_next(node.sequence.back())
        );
        for (auto&& child : children) {
            queue.push_front(child);
        }
        ++fill_counter;
      }
      queue.pop_back();
    }
    assert(c_table[0] == 0);
    for (auto&& k : c_table) {
      assert(k != unassigned);
    }
  };


  void compute_edge_pruning_table(const Block<nc,ne> &b){
    BlockMoveTable m_table(b);
    auto apply = [m_table](const Move& move, CoordinateBlockCube& CBC){m_table.apply(move,CBC);};

    auto unassigned = std::numeric_limits<entry_type>::max();
    e_table.fill(unassigned);
    auto queue = std::deque{Node<CoordinateBlockCube>()};
    uint fill_counter = 0;

    while (fill_counter < e_table_size && queue.size() > 0){
      auto node = queue.back();
      auto table_entry = e_index(node.state);
      assert(table_entry < e_table_size);
      if (e_table[table_entry] == unassigned){
        e_table[table_entry] = node.depth;
        auto children = node.expand(
            apply, allowed_next(node.sequence.back())
        );
        for (auto&& child : children) {
            queue.push_front(child);
        }
        ++fill_counter;
      }
      queue.pop_back();
    }
    assert(e_table[0] == 0);
    for (auto&& k : e_table) {
      assert(k != unassigned);
    }
  };

};
