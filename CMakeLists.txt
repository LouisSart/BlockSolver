cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16)

project(icus CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
export(
  TARGETS block_solver-core
  NAMESPACE gmlib::
  FILE block_solverConfig.cmake)
set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE block_solver)
