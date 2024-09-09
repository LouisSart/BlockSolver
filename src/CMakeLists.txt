add_library(block_solver-core INTERFACE)
add_library(block_solver::core ALIAS block_solver-core)
set_target_properties(block_solver-core PROPERTIES EXPORT_NAME core)
target_sources(block_solver-core INTERFACE
        block.hpp
        coordinate.hpp
        cubie_cube.hpp
        cubie.hpp
        generator.hpp
        move_table.hpp
        move.hpp
        node.hpp
        option.hpp
        search.hpp
        symmetry.hpp
        utils.hpp)
target_include_directories(block_solver-core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
