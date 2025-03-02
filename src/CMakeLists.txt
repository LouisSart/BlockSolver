add_executable(block_solver main.cpp)
target_sources(block_solver PRIVATE
        123.hpp
        222.hpp
        223.hpp
        F2L-1.hpp
        two_gen.hpp
        multistep.hpp
        block.hpp
        generator.hpp
        move_table.hpp
        pruning_table.hpp)

target_include_directories(block_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(block_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../extern/EpiCube/src)