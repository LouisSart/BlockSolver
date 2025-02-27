list(APPEND UNIT_TESTS 2gen block move_table multistep pruning_table)

foreach(f ${UNIT_TESTS})
  set(target ${f}_test)
  add_executable(${target} ${f}_test.cpp)
  target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
  target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../extern/EpiCube/src)
  add_test(NAME ${f}_test COMMAND ${target})
endforeach()
