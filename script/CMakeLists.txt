list(APPEND UNIT_TESTS F2L-1_solve 223_solve three_step_solve compare_pruning_tables)

foreach(f ${UNIT_TESTS})
  set(target ${f})
  add_executable(${target} ${f}.cpp)
  target_link_libraries(${target} PRIVATE block_solver::core)
#   add_test(NAME ${f}_test COMMAND ${target})
endforeach()
