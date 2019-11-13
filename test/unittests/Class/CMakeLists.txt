
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
  javap -c -v '${CMAKE_CURRENT_SOURCE_DIR}/Test.class' > 'Test.diss' &&
  cat 'Test.diss' | tr -s ' ' |
    grep -i 'minor version' | awk '{print $3}' > '${INC_FILE_DIR}/Test_MinorVersion.inc' &&
  cat 'Test.diss' | tr -s ' ' |
    grep -i 'major version' | awk '{print $3}' > '${INC_FILE_DIR}/Test_MajorVersion.inc' &&
  cat 'Test.diss' | tr -s ' ' |
    grep -i 'Utf8 string literal' | awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_StringLiteralIndex.inc' &&
  cat 'Test.diss' | tr -s ' ' |
    grep -i '// testString:Ljava/lang/String;' |  awk '{print $1}' | 
    cut -b 2- > '${INC_FILE_DIR}/Test_StringNameTypeIndex.inc' &&
  cat 'Test.diss' | tr -s ' ' |
    grep -i 'Integer 32768' | awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_IntegerLiteralIndex.inc' &&
  cat 'Test.diss' | tr -s ' ' | grep -i '// testInt:I' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_IntegerNameTypeIndex.inc' &&
  cat 'Test.diss' | tr -s ' ' | grep -i '= Long 2l' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_LongLiteralIndex.inc' &&
  cat 'Test.diss' | tr -s ' ' | grep -i '// testLong' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_LongNameTypeIndex.inc' &&
  cat 'Test.diss' | tr -s ' ' | grep '= Double 2.0d' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_DoubleLiteralIndex.inc' &&
  cat 'Test.diss' | tr -s ' ' | grep '= Float' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_FloatLiteralIndex.inc' &&
  cat 'Test.diss' | tr -s ' ' | grep -i '// Test.testInt' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_IntFieldRefIndex.inc' &&

  javap -v -c '${CMAKE_CURRENT_SOURCE_DIR}/Test.class' | grep 'flags:' |
  awk 'NR==1{acc=$2; if (acc == \"ACC_PUBLIC\") print \"0x0001\"; else if (acc == \"ACC_FINAL\") print \"0x0010\";
      else if (acc == \"ACC_SUPER\") print \"0x0020\"; else if (acc == \"ACC_INTERFACE\") print \"0x0200\" 
      else if (acc == \"ACC_ABSTRACT\") print \"0x0400\"; else if (acc == \"ACC_SYNTHETIC\") print \"0x1000\"
      else if (acc == \"ACC_ANNOTATION\") print \"0x2000\"; else if (acc == \"ACC_ENUM\") print \"0x4000\";
      else exit 1}' > '${INC_FILE_DIR}/Test_AccessFlags.inc' ;

  rm -f '${CMAKE_CURRENT_SOURCE_DIR}/Test.class' 'Test.diss'"
)

if(VERBOSE)
  execute_process(
    COMMAND sh -c "javac '${CMAKE_CURRENT_SOURCE_DIR}/Test.java' &&
      javap -c -v '${CMAKE_CURRENT_SOURCE_DIR}/Test.class' &&
      rm '${CMAKE_CURRENT_SOURCE_DIR}/Test.class'"
  )
endif()


add_test(NAME class_test COMMAND class_test)
