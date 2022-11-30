list(APPEND UNIT_TESTS block)

foreach(f ${UNIT_TESTS})
  set(target ${f}_test)
  add_executable(${target} ${f}_test.cpp)
  target_link_libraries(${target} PRIVATE block_solver::core)
  add_test(NAME ${f}_test COMMAND ${target})
endforeach()
