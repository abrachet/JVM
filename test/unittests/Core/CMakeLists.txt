
add_executable(core_test
    FileBufferTest.cpp
)

include_directories(INCLUDE_DIR)

target_link_libraries(core_test 
    gtest
    gtest_main 
    Core
)

add_test(NAME core_test COMMAND core_test)
