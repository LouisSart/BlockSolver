add_library(block_solver-core INTERFACE)
add_library(block_solver::core ALIAS block_solver-core)
set_target_properties(block_solver-core PROPERTIES EXPORT_NAME core)
target_sources(block_solver-core INTERFACE block_cube.hpp)
target_include_directories(block_solver-core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
