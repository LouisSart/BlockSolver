#pragma once
#include "block_cube.hpp"
#include "algorithm.hpp"
#include <tuple>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

template<uint nc, uint ne>
struct BlockMoveTable
{
  static constexpr uint n_cl = binomial(8, nc);
  static constexpr uint n_cp = factorial(nc);
  static constexpr uint cp_table_size = n_cp*n_cl*18;
  std::array<uint, cp_table_size> cp_table;

  static constexpr uint n_co = ipow(3, nc);
  static constexpr uint co_table_size = n_cl*n_co*18;
  std::array<uint, co_table_size> co_table;

  static constexpr uint n_el = binomial(12, ne);
  static constexpr uint n_ep = factorial(ne);
  static constexpr uint ep_table_size = n_ep*n_el*18;
  std::array<uint, ep_table_size> ep_table;

  static constexpr uint n_eo = ipow(2, ne);
  static constexpr uint eo_table_size = n_el*n_eo*18;
  std::array<uint, eo_table_size> eo_table;

  std::filesystem::path table_dir_path;
  std::filesystem::path block_table_path;
  

  BlockMoveTable(const Block<nc, ne> &b) {

    table_dir_path = fs::current_path() / "move_tables/";
    fs::create_directories(table_dir_path);

    block_table_path = table_dir_path / b.name;
    if (std::filesystem::exists(block_table_path))
    {
      this->load();
    }
    else
    {
      std::cout << "Move table directory not found, building the tables\n";
      std::filesystem::create_directories(block_table_path);
      compute_corner_move_tables(b);
      compute_edge_move_tables(b);
      this->write();
    }
  }

  std::tuple<uint, uint> get_new_ccl_ccp(uint ccl, uint ccp, uint move)
  {
    uint new_cp_idx = cp_table[18*(ccl*n_cp + ccp) + move];
    return std::make_tuple(new_cp_idx / n_cp, new_cp_idx % n_cp);
  }

  std::tuple<uint, uint> get_new_cel_cep(uint cel, uint cep, uint move)
  {
    uint new_ep_idx = ep_table[18*(cel*n_ep + cep) + move];
    return std::make_tuple(new_ep_idx / n_ep, new_ep_idx % n_ep);
  }

  uint get_new_cco(uint ccl, uint cco, uint move){return co_table[18*(ccl*n_co + cco) + move];}
  uint get_new_ceo(uint cel, uint ceo, uint move){return eo_table[18*(cel*n_eo + ceo) + move];}

  void show()
  {
    std::cout << "MoveTable object (" << nc << " corner(s), " << ne << " edge(s))" << '\n';
    std::cout << "  Corner sizes (ncl, ncp, nco): " << n_cl << " " << n_cp << " " << n_co << '\n';
    std::cout << "  Edge sizes (nel, nep, neo): " << n_el << " " << n_ep << " " << n_eo << '\n';
  }
  void write()
  {
    std::filesystem::path cp_table_file = block_table_path / "cp_table.dat";
    std::filesystem::path co_table_file = block_table_path / "co_table.dat";
    std::filesystem::path ep_table_file = block_table_path / "ep_table.dat";
    std::filesystem::path eo_table_file = block_table_path / "eo_table.dat";
    {
      std::ofstream file(cp_table_file, std::ios::binary);
      file.write(reinterpret_cast<char*>(cp_table.data()), sizeof(uint)*cp_table_size);
      file.close();
      file.open(co_table_file, std::ios::binary);
      file.write(reinterpret_cast<char*>(co_table.data()), sizeof(uint)*co_table_size);
      file.close();
      file.open(ep_table_file, std::ios::binary);
      file.write(reinterpret_cast<char*>(ep_table.data()), sizeof(uint)*ep_table_size);
      file.close();
      file.open(eo_table_file, std::ios::binary);
      file.write(reinterpret_cast<char*>(eo_table.data()), sizeof(uint)*eo_table_size);
      file.close();
    }
  }

  void load()
  {
    std::filesystem::path cp_table_path = block_table_path / "cp_table.dat";
    std::filesystem::path co_table_path = block_table_path / "co_table.dat";
    std::filesystem::path ep_table_path = block_table_path / "ep_table.dat";
    std::filesystem::path eo_table_path = block_table_path / "eo_table.dat";

    std::ifstream istrm(cp_table_path, std::ios::binary);
    istrm.read(reinterpret_cast<char*>(cp_table.data()), sizeof(uint)*cp_table_size);
    istrm.close();
    istrm.open(co_table_path, std::ios::binary);
    istrm.read(reinterpret_cast<char*>(co_table.data()), sizeof(uint)*co_table_size);
    istrm.close();
    istrm.open(ep_table_path, std::ios::binary);
    istrm.read(reinterpret_cast<char*>(ep_table.data()), sizeof(uint)*ep_table_size);
    istrm.close();
    istrm.open(eo_table_path, std::ios::binary);
    istrm.read(reinterpret_cast<char*>(eo_table.data()), sizeof(uint)*eo_table_size);
    istrm.close();
  }

  void compute_edge_move_tables(const Block<nc,ne> &b)
  {
    BlockCube<nc,ne> bc(b);
    CubieCube cc, cc_copy;
    CoordinateBlockCube cbc;
    uint ccl=0, cel=0, ccp=0, cep=0, cco=0, ceo=0;

    uint p_idx=0, o_idx=0, m_idx=0;
    for (uint il=0;il<n_el;il++)
    {
      for (uint ip=0;ip<n_ep;ip++) // Loop for the permutation coordinate
      {
        m_idx=0;
        cbc.set(0,il,0,ip,0,0);
        cc = bc.to_cubie_cube(cbc);
        for (auto&& move_idx : HTM_Moves)
        {
          auto move = elementary_transformations[move_idx];
          cc_copy = cc;
          cc_copy.edge_apply(move);
          cbc = bc.to_coordinate_block_cube(cc_copy);
          ep_table[p_idx*18 + m_idx] = cbc.cel*n_ep + cbc.cep;
          m_idx++;
        }
        p_idx ++;
      }
      for (uint io=0;io<n_eo;io++) // Loop for the orientation coordinate
      {
        m_idx=0;
        cbc.set(0,il,0,0,0,io);
        cc = bc.to_cubie_cube(cbc);
        for (auto&& move_idx : HTM_Moves)
        {
          auto move = elementary_transformations[move_idx];
          cc_copy = cc;
          cc_copy.edge_apply(move);
          cbc = bc.to_coordinate_block_cube(cc_copy);
          eo_table[o_idx*18 + m_idx] =  cbc.cel*n_eo + cbc.ceo;
          m_idx++;
        }
        o_idx ++;
      }
    }
  }

  void compute_corner_move_tables(const Block<nc,ne> &b)
  {
    BlockCube<nc,ne> bc(b);
    CubieCube cc, cc_copy;
    CoordinateBlockCube cbc;
    uint ccl=0, cel=0, ccp=0, cep=0, cco=0, ceo=0;

    uint p_idx=0, o_idx=0, m_idx=0;
    for (uint il=0;il<n_cl;il++)
    {
      for (uint ip=0;ip<n_cp;ip++)
      {
        m_idx=0;
        cbc.set(il,0,ip,0,0,0);
        cc = bc.to_cubie_cube(cbc);
        for (auto&& move_idx : HTM_Moves)
        {
          cc_copy = cc;
          auto move = elementary_transformations[move_idx];
          cc_copy.corner_apply(move);
          cbc = bc.to_coordinate_block_cube(cc_copy);
          cp_table[p_idx*18 + m_idx] = cbc.ccl*n_cp + cbc.ccp;
          m_idx++;
        }
        p_idx++;
      }
      for (uint io=0;io<n_co;io++)
      {
        m_idx=0;
        cbc.set(il,0,0,0,io,0);
        cc = bc.to_cubie_cube(cbc);
        for (auto&& move_idx : HTM_Moves)
        {
          auto move = elementary_transformations[move_idx];
          cc_copy = cc;
          cc_copy.corner_apply(move);
          cbc = bc.to_coordinate_block_cube(cc);
          co_table[o_idx*18 + m_idx] = cbc.ccl*n_co + cbc.cco;
          m_idx++;
        }
        o_idx ++;
      }
    }
  }
};
