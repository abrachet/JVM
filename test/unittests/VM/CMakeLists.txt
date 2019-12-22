
add_executable(vm_test
  BasicTest.cpp
)

include_directories(INCLUDE_DIR)

target_link_libraries(vm_test 
  gtest
  gtest_main 
  VM
)

add_test(NAME vm_test COMMAND vm_test)
