
add_executable(class_test
  ClassReaderTest.cpp
)

target_link_libraries(class_test 
  gtest
  gtest_main
  Class
  Core
)

add_custom_command(
  TARGET class_test
  POST_BUILD
    COMMAND javac "${CMAKE_CURRENT_SOURCE_DIR}/Test.java" &&
    mv "${CMAKE_CURRENT_SOURCE_DIR}/Test.class" "$<TARGET_FILE_DIR:class_test>/Test.class"
)

execute_process(
  COMMAND sh -c "javac '${CMAKE_CURRENT_SOURCE_DIR}/Test.java' &&
  javap -c -v '${CMAKE_CURRENT_SOURCE_DIR}/Test.class' | tr -s ' ' |
    grep -i 'minor version' | awk '{print $3}' > '${INC_FILE_DIR}/Test_MinorVersion.inc' &&
  javap -c -v '${CMAKE_CURRENT_SOURCE_DIR}/Test.class' | tr -s ' ' |
    grep -i 'major version' | awk '{print $3}' > '${INC_FILE_DIR}/Test_MajorVersion.inc' &&
  javap -c -v '${CMAKE_CURRENT_SOURCE_DIR}/Test.class' | tr -s ' ' |
    grep -i 'Utf8 string literal' | awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_StringLiteralIndex.inc' &&
  rm '${CMAKE_CURRENT_SOURCE_DIR}/Test.class'"
)

add_test(NAME class_test COMMAND class_test)
