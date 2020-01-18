
add_executable(vm_test
  JNITest.cpp
  StackTest.cpp
)

include_directories(INCLUDE_DIR)

target_link_libraries(vm_test 
  gtest
  gtest_main 
  VM
)

add_custom_command(
  TARGET vm_test
  POST_BUILD
    COMMAND ${CMAKE_CXX_COMPILER} "${CMAKE_CURRENT_SOURCE_DIR}/ExternSym.cpp" -shared -o
      "$<TARGET_FILE_DIR:vm_test>/ExternSym.so"
)


add_test(NAME vm_test COMMAND vm_test)
