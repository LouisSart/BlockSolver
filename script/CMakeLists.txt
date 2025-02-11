set(SCRIPT_HEADERS
    123.hpp
    222.hpp
    223.hpp
    F2L-1.hpp
)

install(FILES ${SCRIPT_HEADERS} DESTINATION include/block_solver/script)

add_executable(multistep multistep.cpp)
target_include_directories(multistep PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_include_directories(multistep PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../extern/EpiCube/src)