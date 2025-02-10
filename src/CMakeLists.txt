add_executable(block_solver main.cpp)
target_sources(block_solver PRIVATE
        block.hpp
        generator.hpp
        move_table.hpp
        pruning_table.hpp)

target_include_directories(block_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(block_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../script)
target_include_directories(block_solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../extern/EpiCube/src)