# list(APPEND UNIT_TESTS F2L-1_solve 223_solve three_step_solve zz_skeleton)
list(APPEND UNIT_TESTS 222 123 223 F2L-1)

foreach(f ${UNIT_TESTS})
  set(target ${f})
  add_executable(${target} ${f}.cpp)
  target_link_libraries(${target} PRIVATE block_solver::core)
#   add_test(NAME ${f}_test COMMAND ${target})
endforeach()
