#include "move_table.hpp"

template<uint nc, uint ne>
class PruningTable
{
private:
  static constexpr uint c_table_size = binomial(8, nc)*factorial(nc)*ipow(3, nc);
  std::array<uint, c_table_size> c_table;
  static constexpr uint e_table_size = binomial(12, ne)*factorial(ne)*ipow(2, ne);
  std::array<uint, e_table_size> e_table;

  std::string name;
  std::filesystem::path table_dir_path{"/home/epicier/Documents/Rubik's Cube/FirstCppProject/pruning_tables/"};
  std::filesystem::path block_table_path;

public:
  PruningTable(){};
  PruningTable(const Block<nc,ne> &b)
  {
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
      file.write(reinterpret_cast<char*>(c_table.data()), sizeof(uint)*c_table_size);
      file.close();
      file.open(e_table_file, std::ios::binary);
      file.write(reinterpret_cast<char*>(e_table.data()), sizeof(uint)*e_table_size);
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

  void compute_edge_pruning_table(const Block<nc,ne>){};
  void compute_corner_pruning_table(const Block<nc,ne>){};
};
