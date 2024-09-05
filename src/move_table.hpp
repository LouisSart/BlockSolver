#pragma once
#include <cassert>     // assert
#include <filesystem>  // locate move table files
#include <fstream>     // write tabes into files
#include <memory>      // std::unique_ptr
#include <tuple>       // return ccl and ccp at the same time

#include "algorithm.hpp"  // apply Algorithm
#include "block.hpp"

namespace fs = std::filesystem;
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

template <unsigned nc, unsigned ne>
struct BlockMoveTable {
    static constexpr unsigned n_cl = binomial(NC, nc);
    static constexpr unsigned n_cp = factorial(nc);
    static constexpr unsigned cp_table_size = n_cp * n_cl * N_HTM_MOVES;
    std::unique_ptr<unsigned[]> cp_table{new unsigned[cp_table_size]};

    static constexpr unsigned n_co = ipow(3, nc);
    static constexpr unsigned co_table_size = n_cl * n_co * N_HTM_MOVES;
    std::unique_ptr<unsigned[]> co_table{new unsigned[co_table_size]};

    static constexpr unsigned n_el = binomial(NE, ne);
    static constexpr unsigned n_ep = factorial(ne);
    static constexpr unsigned ep_table_size = n_ep * n_el * N_HTM_MOVES;
    std::unique_ptr<unsigned[]> ep_table{new unsigned[ep_table_size]};

    static constexpr unsigned n_eo = ipow(2, ne);
    static constexpr unsigned eo_table_size = n_el * n_eo * N_HTM_MOVES;
    std::unique_ptr<unsigned[]> eo_table{new unsigned[eo_table_size]};

    BlockMoveTable() {}
    BlockMoveTable(Block<nc, ne>& b) {
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
        apply(inverse_of_HTM_Moves[move], cbc);
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

    auto sym_apply(const Move& move, const unsigned& sym_index,
                   CoordinateBlockCube& cube) const {
        apply(move_conj(move, sym_index), cube);
    }

    auto sym_apply(const Algorithm& alg, const unsigned& sym_index,
                   CoordinateBlockCube& cube) const {
        for (auto move : alg.sequence) {
            sym_apply(move, sym_index, cube);
        };
    }

    std::tuple<unsigned, unsigned> get_new_ccl_ccp(unsigned ccl, unsigned ccp,
                                                   unsigned move) const {
        assert(N_HTM_MOVES * (ccl * n_cp + ccp) + move < cp_table_size);
        unsigned new_cp_idx = cp_table[N_HTM_MOVES * (ccl * n_cp + ccp) + move];
        return std::make_tuple(new_cp_idx / n_cp, new_cp_idx % n_cp);
    }

    std::tuple<unsigned, unsigned> get_new_cel_cep(unsigned cel, unsigned cep,
                                                   unsigned move) const {
        assert(N_HTM_MOVES * (cel * n_ep + cep) + move < ep_table_size);
        unsigned new_ep_idx = ep_table[N_HTM_MOVES * (cel * n_ep + cep) + move];
        return std::make_tuple(new_ep_idx / n_ep, new_ep_idx % n_ep);
    }

    unsigned get_new_cco(unsigned ccl, unsigned cco, unsigned move) const {
        assert(N_HTM_MOVES * (ccl * n_co + cco) + move < co_table_size);
        return co_table[N_HTM_MOVES * (ccl * n_co + cco) + move];
    }

    unsigned get_new_ceo(unsigned cel, unsigned ceo, unsigned move) const {
        assert(N_HTM_MOVES * (cel * n_eo + ceo) + move < eo_table_size);
        return eo_table[N_HTM_MOVES * (cel * n_eo + ceo) + move];
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

        write_binary<unsigned>(cp_table_file, cp_table.get(), cp_table_size);
        write_binary<unsigned>(co_table_file, co_table.get(), co_table_size);
        write_binary<unsigned>(ep_table_file, ep_table.get(), ep_table_size);
        write_binary<unsigned>(eo_table_file, eo_table.get(), eo_table_size);
    }

    void load(const std::filesystem::path& table_path) const {
        assert(fs::exists(table_path));
        std::filesystem::path cp_table_path = table_path / "cp_table.dat";
        std::filesystem::path co_table_path = table_path / "co_table.dat";
        std::filesystem::path ep_table_path = table_path / "ep_table.dat";
        std::filesystem::path eo_table_path = table_path / "eo_table.dat";

        load_binary<unsigned>(cp_table_path, cp_table.get(), cp_table_size);
        load_binary<unsigned>(co_table_path, co_table.get(), co_table_size);
        load_binary<unsigned>(ep_table_path, ep_table.get(), ep_table_size);
        load_binary<unsigned>(eo_table_path, eo_table.get(), eo_table_size);
    }

    void compute_edge_move_tables(Block<nc, ne>& b) {
        CubieCube cc, cc_copy;
        CoordinateBlockCube cbc;

        unsigned p_idx = 0, o_idx = 0, m_idx = 0;
        for (unsigned il = 0; il < n_el; il++) {
            // Loop for the permutation coordinate
            for (unsigned ip = 0; ip < n_ep; ip++) {
                m_idx = 0;
                cbc.set(0, il, 0, ip, 0, 0);
                cc = b.to_cubie_cube(cbc);
                for (auto&& move_idx : HTM_Moves) {
                    auto move = move_cc[move_idx];
                    cc_copy = cc;
                    cc_copy.edge_apply(move);
                    cbc = b.to_coordinate_block_cube(cc_copy);
                    assert(N_HTM_MOVES * (cbc.cel * n_ep + cbc.cep) <
                           ep_table_size);
                    assert(p_idx * N_HTM_MOVES + m_idx < ep_table_size);
                    ep_table[p_idx * N_HTM_MOVES + m_idx] =
                        cbc.cel * n_ep + cbc.cep;
                    m_idx++;
                }
                p_idx++;
            }
            for (unsigned io = 0; io < n_eo;
                 io++)  // Loop for the orientation coordinate
            {
                m_idx = 0;
                cbc.set(0, il, 0, 0, 0, io);
                cc = b.to_cubie_cube(cbc);
                for (auto&& move_idx : HTM_Moves) {
                    auto move = move_cc[move_idx];
                    cc_copy = cc;
                    cc_copy.edge_apply(move);
                    cbc = b.to_coordinate_block_cube(cc_copy);
                    assert(N_HTM_MOVES * (cbc.cel * n_eo + cbc.ceo) <
                           eo_table_size);
                    assert(o_idx * N_HTM_MOVES + m_idx < eo_table_size);
                    eo_table[o_idx * N_HTM_MOVES + m_idx] = cbc.ceo;
                    m_idx++;
                }
                o_idx++;
            }
        }
    }

    void compute_corner_move_tables(Block<nc, ne>& b) {
        Block<nc, ne> bc(b);
        CubieCube cc, cc_copy;
        CoordinateBlockCube cbc;

        unsigned p_idx = 0, o_idx = 0, m_idx = 0;
        for (unsigned il = 0; il < n_cl; il++) {
            for (unsigned ip = 0; ip < n_cp; ip++) {
                m_idx = 0;
                cbc.set(il, 0, ip, 0, 0, 0);
                cc = b.to_cubie_cube(cbc);
                for (auto&& move_idx : HTM_Moves) {
                    cc_copy = cc;
                    auto move = move_cc[move_idx];
                    cc_copy.corner_apply(move);
                    cbc = b.to_coordinate_block_cube(cc_copy);
                    assert(N_HTM_MOVES * (cbc.ccl * n_cp + cbc.ccp) <
                           cp_table_size);
                    assert(p_idx * N_HTM_MOVES + m_idx < cp_table_size);
                    cp_table[p_idx * N_HTM_MOVES + m_idx] =
                        cbc.ccl * n_cp + cbc.ccp;
                    m_idx++;
                }
                p_idx++;
            }
            for (unsigned io = 0; io < n_co; io++) {
                m_idx = 0;
                cbc.set(il, 0, 0, 0, io, 0);
                cc = bc.to_cubie_cube(cbc);
                for (auto&& move_idx : HTM_Moves) {
                    auto move = move_cc[move_idx];
                    cc_copy = cc;
                    cc_copy.corner_apply(move);
                    cbc = bc.to_coordinate_block_cube(cc_copy);
                    assert(N_HTM_MOVES * (cbc.ccl * n_co + cbc.cco) <
                           co_table_size);
                    assert(o_idx * N_HTM_MOVES + m_idx < co_table_size);
                    co_table[o_idx * N_HTM_MOVES + m_idx] = cbc.cco;
                    m_idx++;
                }
                o_idx++;
            }
        }
    }
};

struct EOMoveTable {
    static constexpr unsigned table_size = ipow(2, NE - 1) * N_HTM_MOVES;
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
        assert(fs::exists(table_path()));
        load_binary<unsigned>(table_path() / "table.dat", table.get(),
                              table_size);
    }
    void write() const {
        fs::create_directories(table_path());
        write_binary<unsigned>(table_path() / "table.dat", table.get(),
                               table_size);
    }
    void compute_table() {
        CubieCube cube, tmp;

        for (unsigned eo_c = 0; eo_c < ipow(2, NE - 1); ++eo_c) {
            cube.eo[NE - 1] = 0;
            eo_from_index(eo_c, cube.eo);
            if (!cube.has_consistent_eo()) {
                cube.eo[NE - 1] = 1;
            }
            assert(cube.has_consistent_eo());
            for (Move move : HTM_Moves) {
                tmp = cube;
                tmp.apply(move);
                assert(eo_c * N_HTM_MOVES + move < table_size);
                assert((eo_index<NE, true>(tmp.eo) < ipow(2, NE - 1)));
                table[eo_c * N_HTM_MOVES + move] = eo_index<NE, true>(tmp.eo);
            }
        }
    }

    auto apply(const Move& move, CoordinateBlockCube& cube) const {
        assert(cube.ceo * N_HTM_MOVES + move < table_size);
        cube.ceo = table[cube.ceo * N_HTM_MOVES + move];
    }

    auto apply(const Algorithm& alg, CoordinateBlockCube& cube) const {
        for (auto move : alg.sequence) {
            apply(move, cube);
        };
    }
    void apply_inverse(const Move& move, CoordinateBlockCube& cube) const {
        apply(inverse_of_HTM_Moves[move], cube);
    }

    void apply_inverse(const Algorithm& alg, CoordinateBlockCube& cube) const {
        for (auto move = alg.sequence.rbegin(); move != alg.sequence.rend();
             ++move) {
            apply_inverse(*move, cube);
        }
    }

    auto sym_apply(const Move& move, const unsigned& sym_index,
                   CoordinateBlockCube& cube) const {
        apply(move_conj(move, sym_index), cube);
    }

    auto sym_apply(const Algorithm& alg, const unsigned& sym_index,
                   CoordinateBlockCube& cube) const {
        for (auto move : alg.sequence) {
            sym_apply(move, sym_index, cube);
        };
    }
};