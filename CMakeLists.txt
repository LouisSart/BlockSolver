cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(block_solver CXX)
include(CTest)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Debug"
      CACHE
        STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(script)
add_subdirectory(extern/EpiCube)

# Link the block_solver executable to the epicube-core library
# target_link_libraries(block_solver PRIVATE epicube-core)