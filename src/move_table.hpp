#pragma once
#include <filesystem>  // locate move table files
#include <fstream>     // write tabes into files
#include <memory>      // std::unique_ptr
#include <tuple>       // return ccl and ccp at the same time

#include "algorithm.hpp"
#include "block_cube.hpp"

namespace fs = std::filesystem;

template <uint nc, uint ne>
struct BlockMoveTable {
    static constexpr uint n_cl = binomial(NC, nc);
    static constexpr uint n_cp = factorial(nc);
    static constexpr uint cp_table_size =
        n_cp * n_cl * N_HTM_MOVES_AND_ROTATIONS;
    std::unique_ptr<uint[]> cp_table{new uint[cp_table_size]};

    static constexpr uint n_co = ipow(3, nc);
    static constexpr uint co_table_size =
        n_cl * n_co * N_HTM_MOVES_AND_ROTATIONS;
    std::unique_ptr<uint[]> co_table{new uint[co_table_size]};

    static constexpr uint n_el = binomial(NE, ne);
    static constexpr uint n_ep = factorial(ne);
    static constexpr uint ep_table_size =
        n_ep * n_el * N_HTM_MOVES_AND_ROTATIONS;
    std::unique_ptr<uint[]> ep_table{new uint[ep_table_size]};

    static constexpr uint n_eo = ipow(2, ne);
    static constexpr uint eo_table_size =
        n_el * n_eo * N_HTM_MOVES_AND_ROTATIONS;
    std::unique_ptr<uint[]> eo_table{new uint[eo_table_size]};

    BlockMoveTable() {}
    BlockMoveTable(const Block<nc, ne>& b) {
        auto table_path = block_table_path(b);
        if (fs::exists(table_path)) {
            this->load(table_path);
        } else {
            std::cout
                << "Move table directory not found, building the tables\n";
            compute_corner_move_tables(b);
            compute_edge_move_tables(b);
            this->write(table_path);
        }
    }

    auto table_dir_path() const { return fs::current_path() / "move_tables/"; }
    auto block_table_path(const Block<nc, ne>& b) const {
        return table_dir_path() / b.id;
    }

    void apply(const unsigned move, CoordinateBlockCube& cbc) const {
        auto&& [ccl, ccp] = get_new_ccl_ccp(cbc.ccl, cbc.ccp, move);
        auto&& cco = get_new_cco(cbc.ccl, cbc.cco, move);
        auto&& [cel, cep] = get_new_cel_cep(cbc.cel, cbc.cep, move);
        auto&& ceo = get_new_ceo(cbc.cel, cbc.ceo, move);
        cbc.set(ccl, cel, ccp, cep, cco, ceo);
    }

    void apply_inverse(const unsigned& move, CoordinateBlockCube& cbc) const {
        apply(inverse_of_HTM_Moves_and_rotations[move], cbc);
    }

    void apply_inverse(const Algorithm& alg, CoordinateBlockCube& cbc) const {
        for (auto move = alg.sequence.rbegin(); move != alg.sequence.rend();
             ++move) {
            apply_inverse(*move, cbc);
        }
    }

    void apply(const Algorithm& alg, CoordinateBlockCube& cbc) const {
        for (const auto& move : alg.sequence) {
            apply(move, cbc);
        }
    }

    std::tuple<uint, uint> get_new_ccl_ccp(uint ccl, uint ccp,
                                           uint move) const {
        assert(N_HTM_MOVES_AND_ROTATIONS * (ccl * n_cp + ccp) + move <
               cp_table_size);
        uint new_cp_idx =
            cp_table[N_HTM_MOVES_AND_ROTATIONS * (ccl * n_cp + ccp) + move];
        return std::make_tuple(new_cp_idx / n_cp, new_cp_idx % n_cp);
    }

    std::tuple<uint, uint> get_new_cel_cep(uint cel, uint cep,
                                           uint move) const {
        assert(N_HTM_MOVES_AND_ROTATIONS * (cel * n_ep + cep) + move <
               ep_table_size);
        uint new_ep_idx =
            ep_table[N_HTM_MOVES_AND_ROTATIONS * (cel * n_ep + cep) + move];
        return std::make_tuple(new_ep_idx / n_ep, new_ep_idx % n_ep);
    }

    uint get_new_cco(uint ccl, uint cco, uint move) const {
        assert(N_HTM_MOVES_AND_ROTATIONS * (ccl * n_co + cco) + move <
               co_table_size);
        return co_table[N_HTM_MOVES_AND_ROTATIONS * (ccl * n_co + cco) + move];
    }

    uint get_new_ceo(uint cel, uint ceo, uint move) const {
        assert(N_HTM_MOVES_AND_ROTATIONS * (cel * n_eo + ceo) + move <
               eo_table_size);
        return eo_table[N_HTM_MOVES_AND_ROTATIONS * (cel * n_eo + ceo) + move];
    }

    void show() const {
        std::cout << "MoveTable object (" << nc << " corner(s), " << ne
                  << " edge(s))" << '\n';
        std::cout << "  Corner sizes (ncl, ncp, nco): " << n_cl << " " << n_cp
                  << " " << n_co << '\n';
        std::cout << "  Edge sizes (nel, nep, neo): " << n_el << " " << n_ep
                  << " " << n_eo << '\n';
    }
    void write(const std::filesystem::path& table_path) const {
        fs::create_directories(table_path);
        std::filesystem::path cp_table_file = table_path / "cp_table.dat";
        std::filesystem::path co_table_file = table_path / "co_table.dat";
        std::filesystem::path ep_table_file = table_path / "ep_table.dat";
        std::filesystem::path eo_table_file = table_path / "eo_table.dat";
        {
            std::ofstream file(cp_table_file, std::ios::binary);
            file.write(reinterpret_cast<char*>(cp_table.get()),
                       sizeof(uint) * cp_table_size);
            file.close();
            file.open(co_table_file, std::ios::binary);
            file.write(reinterpret_cast<char*>(co_table.get()),
                       sizeof(uint) * co_table_size);
            file.close();
            file.open(ep_table_file, std::ios::binary);
            file.write(reinterpret_cast<char*>(ep_table.get()),
                       sizeof(uint) * ep_table_size);
            file.close();
            file.open(eo_table_file, std::ios::binary);
            file.write(reinterpret_cast<char*>(eo_table.get()),
                       sizeof(uint) * eo_table_size);
            file.close();
        }
    }

    void load(const std::filesystem::path& table_path) const {
        assert(fs::exists(table_path));
        std::filesystem::path cp_table_path = table_path / "cp_table.dat";
        std::filesystem::path co_table_path = table_path / "co_table.dat";
        std::filesystem::path ep_table_path = table_path / "ep_table.dat";
        std::filesystem::path eo_table_path = table_path / "eo_table.dat";

        std::ifstream istrm(cp_table_path, std::ios::binary);
        istrm.read(reinterpret_cast<char*>(cp_table.get()),
                   sizeof(uint) * cp_table_size);
        istrm.close();
        istrm.open(co_table_path, std::ios::binary);
        istrm.read(reinterpret_cast<char*>(co_table.get()),
                   sizeof(uint) * co_table_size);
        istrm.close();
        istrm.open(ep_table_path, std::ios::binary);
        istrm.read(reinterpret_cast<char*>(ep_table.get()),
                   sizeof(uint) * ep_table_size);
        istrm.close();
        istrm.open(eo_table_path, std::ios::binary);
        istrm.read(reinterpret_cast<char*>(eo_table.get()),
                   sizeof(uint) * eo_table_size);
        istrm.close();
    }

    void compute_edge_move_tables(const Block<nc, ne>& b) {
        BlockCube<nc, ne> bc(b);
        CubieCube cc, cc_copy;
        CoordinateBlockCube cbc;
        uint ccl = 0, cel = 0, ccp = 0, cep = 0, cco = 0, ceo = 0;

        uint p_idx = 0, o_idx = 0, m_idx = 0;
        for (uint il = 0; il < n_el; il++) {
            // Loop for the permutation coordinate
            for (uint ip = 0; ip < n_ep; ip++) {
                m_idx = 0;
                cbc.set(0, il, 0, ip, 0, 0);
                cc = bc.to_cubie_cube(cbc);
                for (auto&& move_idx : HTM_Moves_and_rotations) {
                    auto move = elementary_transformations[move_idx];
                    cc_copy = cc;
                    cc_copy.edge_apply(move);
                    cbc = bc.to_coordinate_block_cube(cc_copy);
                    assert(N_HTM_MOVES_AND_ROTATIONS *
                               (cbc.cel * n_ep + cbc.cep) <
                           ep_table_size);
                    assert(p_idx * N_HTM_MOVES_AND_ROTATIONS + m_idx <
                           ep_table_size);
                    ep_table[p_idx * N_HTM_MOVES_AND_ROTATIONS + m_idx] =
                        cbc.cel * n_ep + cbc.cep;
                    m_idx++;
                }
                p_idx++;
            }
            for (uint io = 0; io < n_eo;
                 io++)  // Loop for the orientation coordinate
            {
                m_idx = 0;
                cbc.set(0, il, 0, 0, 0, io);
                cc = bc.to_cubie_cube(cbc);
                for (auto&& move_idx : HTM_Moves_and_rotations) {
                    auto move = elementary_transformations[move_idx];
                    cc_copy = cc;
                    cc_copy.edge_apply(move);
                    cbc = bc.to_coordinate_block_cube(cc_copy);
                    assert(N_HTM_MOVES_AND_ROTATIONS *
                               (cbc.cel * n_eo + cbc.ceo) <
                           eo_table_size);
                    assert(o_idx * N_HTM_MOVES_AND_ROTATIONS + m_idx <
                           eo_table_size);
                    eo_table[o_idx * N_HTM_MOVES_AND_ROTATIONS + m_idx] =
                        cbc.ceo;
                    m_idx++;
                }
                o_idx++;
            }
        }
    }

    void compute_corner_move_tables(const Block<nc, ne>& b) {
        BlockCube<nc, ne> bc(b);
        CubieCube cc, cc_copy;
        CoordinateBlockCube cbc;
        uint ccl = 0, cel = 0, ccp = 0, cep = 0, cco = 0, ceo = 0;

        uint p_idx = 0, o_idx = 0, m_idx = 0;
        for (uint il = 0; il < n_cl; il++) {
            for (uint ip = 0; ip < n_cp; ip++) {
                m_idx = 0;
                cbc.set(il, 0, ip, 0, 0, 0);
                cc = bc.to_cubie_cube(cbc);
                for (auto&& move_idx : HTM_Moves_and_rotations) {
                    cc_copy = cc;
                    auto move = elementary_transformations[move_idx];
                    cc_copy.corner_apply(move);
                    cbc = bc.to_coordinate_block_cube(cc_copy);
                    assert(N_HTM_MOVES_AND_ROTATIONS *
                               (cbc.ccl * n_cp + cbc.ccp) <
                           cp_table_size);
                    assert(p_idx * N_HTM_MOVES_AND_ROTATIONS + m_idx <
                           cp_table_size);
                    cp_table[p_idx * N_HTM_MOVES_AND_ROTATIONS + m_idx] =
                        cbc.ccl * n_cp + cbc.ccp;
                    m_idx++;
                }
                p_idx++;
            }
            for (uint io = 0; io < n_co; io++) {
                m_idx = 0;
                cbc.set(il, 0, 0, 0, io, 0);
                cc = bc.to_cubie_cube(cbc);
                for (auto&& move_idx : HTM_Moves_and_rotations) {
                    auto move = elementary_transformations[move_idx];
                    cc_copy = cc;
                    cc_copy.corner_apply(move);
                    cbc = bc.to_coordinate_block_cube(cc_copy);
                    assert(N_HTM_MOVES_AND_ROTATIONS *
                               (cbc.ccl * n_co + cbc.cco) <
                           co_table_size);
                    assert(o_idx * N_HTM_MOVES_AND_ROTATIONS + m_idx <
                           co_table_size);
                    co_table[o_idx * N_HTM_MOVES_AND_ROTATIONS + m_idx] =
                        cbc.cco;
                    m_idx++;
                }
                o_idx++;
            }
        }
    }
};

template <typename value_type>
void load_binary(const std::filesystem::path& table_path, value_type* ptr,
                 size_t size) {
    std::ifstream istrm(table_path, std::ios::binary);
    istrm.read(reinterpret_cast<char*>(ptr), sizeof(value_type) * size);
    istrm.close();
}

template <typename value_type>
void write_binary(const std::filesystem::path& table_path, value_type* ptr,
                  size_t size) {
    std::ofstream file(table_path, std::ios::binary);
    file.write(reinterpret_cast<char*>(ptr), sizeof(value_type) * size);
    file.close();
}

struct EOMoveTable {
    static constexpr unsigned table_size =
        ipow(2, NE - 1) * N_HTM_MOVES_AND_ROTATIONS;
    std::unique_ptr<unsigned[]> table{new unsigned[table_size]};

    EOMoveTable() {
        if (fs::exists(table_path())) {
            this->load();
        } else {
            std::cout
                << "EO move table directory not found, building the table\n";
            compute_table();
            this->write();
        };
    }

    std::filesystem::path table_dir_path() const {
        return fs::current_path() / "move_tables/";
    }
    std::filesystem::path table_path() const { return table_dir_path() / "eo"; }
    void load() const {
        load_binary<unsigned>(table_path(), table.get(), table_size);
    }
    void write() const {
        write_binary<unsigned>(table_path(), table.get(), table_size);
    }
    void compute_table() {
        CubieCube cube, tmp;

        for (unsigned eo_c = 0; eo_c < ipow(2, NE - 1); ++eo_c) {
            cube.eo[NE - 1] = 0;
            eo_from_coord(eo_c, NE - 1, cube.eo);
            if (!cube.has_consistent_eo()) {
                cube.eo[NE - 1] = 1;
            }
            assert(cube.has_consistent_eo());
            for (Move move : HTM_Moves_and_rotations) {
                tmp = cube;
                tmp.apply(move);
                assert(eo_c * N_HTM_MOVES_AND_ROTATIONS + move < table_size);
                table[eo_c * N_HTM_MOVES_AND_ROTATIONS + move] =
                    eo_coord(tmp.eo, NE - 1);
            }
        }
    }

    auto apply(const Move& move, EOCube& cube) const {
        assert(cube.ceo * N_HTM_MOVES_AND_ROTATIONS + move < table_size);
        cube.ceo = table[cube.ceo * N_HTM_MOVES_AND_ROTATIONS + move];
    }

    auto apply(const Algorithm& alg, EOCube& cube) const {
        for (auto move : alg.sequence) {
            apply(move, cube);
        };
    }
    void apply_inverse(const Move& move, EOCube& cube) const {
        apply(inverse_of_HTM_Moves_and_rotations[move], cube);
    }

    void apply_inverse(const Algorithm& alg, EOCube& cube) const {
        for (auto move = alg.sequence.rbegin(); move != alg.sequence.rend();
             ++move) {
            apply_inverse(*move, cube);
        }
    }
};
