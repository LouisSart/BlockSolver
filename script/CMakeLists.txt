list(APPEND UNIT_TESTS F2L-1_solve 223_solve solve_all_12_2x2x3 three_step_solve)

foreach(f ${UNIT_TESTS})
  set(target ${f})
  add_executable(${target} ${f}.cpp)
  target_link_libraries(${target} PRIVATE block_solver::core)
#   add_test(NAME ${f}_test COMMAND ${target})
endforeach()
