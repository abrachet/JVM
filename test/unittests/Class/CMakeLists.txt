
add_executable(class_test
  ClassFinderTest.cpp
  ClassLoaderTest.cpp
  ClassReaderCPBasicTest.cpp
  ClassReaderCPMethodsTest.cpp
  ClassReaderExceptionTest.cpp
  ClassReaderFieldsTest.cpp
  ClassReaderInterfaceTest.cpp
  ClassReaderMethodsTest.cpp
)

target_link_libraries(class_test 
  gtest
  gtest_main
  Class
  Core
  VM
)

execute_process(
  COMMAND javap -version
  OUTPUT_VARIABLE JAVAP_VERSION
)


if (JAVAP_VERSION MATCHES "1.8")
  set(OLD_JDK TRUE)
endif()

add_custom_command(
  TARGET class_test
  POST_BUILD
    COMMAND javac "${CMAKE_CURRENT_SOURCE_DIR}/Basic.java" &&
    mv "${CMAKE_CURRENT_SOURCE_DIR}/Basic.class" "$<TARGET_FILE_DIR:class_test>/Basic.class"
    COMMAND javac "${CMAKE_CURRENT_SOURCE_DIR}/InvD.java" &&
    mv "${CMAKE_CURRENT_SOURCE_DIR}/InvD.class" "$<TARGET_FILE_DIR:class_test>/InvD.class"
    COMMAND javac "${CMAKE_CURRENT_SOURCE_DIR}/Interface.java" &&
    mv "${CMAKE_CURRENT_SOURCE_DIR}/Interface.class" "$<TARGET_FILE_DIR:class_test>/Interface.class" &&
    mv "${CMAKE_CURRENT_SOURCE_DIR}/C.class" "$<TARGET_FILE_DIR:class_test>/C.class"
    COMMAND javac "${CMAKE_CURRENT_SOURCE_DIR}/Methods.java" &&
    mv "${CMAKE_CURRENT_SOURCE_DIR}/Methods.class" "$<TARGET_FILE_DIR:class_test>/Methods.class"
    COMMAND javac "${CMAKE_CURRENT_SOURCE_DIR}/ExceptionTable.java" &&
    mv "${CMAKE_CURRENT_SOURCE_DIR}/ExceptionTable.class" "$<TARGET_FILE_DIR:class_test>/ExceptionTable.class"
)

execute_process(
  COMMAND sh -c "javac '${CMAKE_CURRENT_SOURCE_DIR}/Basic.java' &&
  javap -c -v '${CMAKE_CURRENT_SOURCE_DIR}/Basic.class' > 'Basic.diss' &&
  cat 'Basic.diss' | tr -s ' ' |
    grep -i 'minor version' | awk '{print $3}' > '${INC_FILE_DIR}/Test_MinorVersion.inc' &&
  cat 'Basic.diss' | tr -s ' ' |
    grep -i 'major version' | awk '{print $3}' > '${INC_FILE_DIR}/Test_MajorVersion.inc' &&
  cat 'Basic.diss' | tr -s ' ' |
    grep -i 'Utf8 string literal' | awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_StringLiteralIndex.inc' &&
  cat 'Basic.diss' | tr -s ' ' |
    grep -i '// testString:Ljava/lang/String;' |  awk '{print $1}' | 
    cut -b 2- > '${INC_FILE_DIR}/Test_StringNameTypeIndex.inc' &&
  cat 'Basic.diss' | tr -s ' ' |
    grep -i 'Integer 32768' | awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_IntegerLiteralIndex.inc' &&
  cat 'Basic.diss' | tr -s ' ' | grep -i '// testInt:I' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_IntegerNameTypeIndex.inc' &&
  cat 'Basic.diss' | tr -s ' ' | grep -i '= Long 2l' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_LongLiteralIndex.inc' &&
  cat 'Basic.diss' | tr -s ' ' | grep -i '// testLong' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_LongNameTypeIndex.inc' &&
  cat 'Basic.diss' | tr -s ' ' | grep '= Double 2.0d' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_DoubleLiteralIndex.inc' &&
  cat 'Basic.diss' | tr -s ' ' | grep '= Float' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_FloatLiteralIndex.inc' &&
  cat 'Basic.diss' | tr -s ' ' | grep -i '// Basic.testInt' |
    awk '{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Test_IntFieldRefIndex.inc' &&


  rm -f '${CMAKE_CURRENT_SOURCE_DIR}/Basic.class' 'Basic.diss'"
)

## TODO: 
if(OLD_JDK)
  execute_process(
    COMMAND sh -c "javac '${CMAKE_CURRENT_SOURCE_DIR}/Basic.java' &&
    javap -c -v '${CMAKE_CURRENT_SOURCE_DIR}/Basic.class' | grep 'flags:' |
      awk 'NR==1{acc=$2; if (acc == \"ACC_PUBLIC\") print \"0x0001\"; else if (acc == \"ACC_FINAL\") print \"0x0010\";
      else if (acc == \"ACC_SUPER\") print \"0x0020\"; else if (acc == \"ACC_INTERFACE\") print \"0x0200\";
      else if (acc == \"ACC_ABSTRACT\") print \"0x0400\"; else if (acc == \"ACC_SYNTHETIC\") print \"0x1000\";
      else if (acc == \"ACC_ANNOTATION\") print \"0x2000\"; else if (acc == \"ACC_ENUM\") print \"0x4000\";
      else exit 1}' > '${INC_FILE_DIR}/Test_AccessFlags.inc' &&
    rm -rf '${CMAKE_CURRENT_SOURCE_DIR}/Basic.class'"
  )
else()
  execute_process(
    COMMAND sh -c "javac '${CMAKE_CURRENT_SOURCE_DIR}/Basic.java' &&
    javap -c -v '${CMAKE_CURRENT_SOURCE_DIR}/Basic.class' | grep 'flags:' |
      awk 'NR==1{acc=$3; if (acc == \"ACC_PUBLIC\") print \"0x0001\"; else if (acc == \"ACC_FINAL\") print \"0x0010\";
      else if (acc == \"ACC_SUPER\") print \"0x0020\"; else if (acc == \"ACC_INTERFACE\") print \"0x0200\";
      else if (acc == \"ACC_ABSTRACT\") print \"0x0400\"; else if (acc == \"ACC_SYNTHETIC\") print \"0x1000\";
      else if (acc == \"ACC_ANNOTATION\") print \"0x2000\"; else if (acc == \"ACC_ENUM\") print \"0x4000\";
      else exit 1}' > '${INC_FILE_DIR}/Test_AccessFlags.inc' &&
    rm -rf '${CMAKE_CURRENT_SOURCE_DIR}/Basic.class'"
  )
endif()

execute_process(
  COMMAND sh -c "javac '${CMAKE_CURRENT_SOURCE_DIR}/InvD.java' &&
  javap -c -v '${CMAKE_CURRENT_SOURCE_DIR}/InvD.class' > InvD.diss &&

  cat InvD.diss | tr -s ' ' | grep '= InvokeDynamic' | awk '{print $1}' |
    cut -b 2- > '${INC_FILE_DIR}/InvD_InvokeDynamicIndex.inc' &&
  cat InvD.diss | tr -s ' ' | grep '= InvokeDynamic' | awk '{print $4}' |
    cut -f 1 -d : | cut -b 2- > '${INC_FILE_DIR}/InvD_InvDBSMethod.inc' &&
  cat InvD.diss | tr -s ' ' | grep '= InvokeDynamic' | awk '{print $4}' |
    cut -f 2 -d : | cut -b 2- > '${INC_FILE_DIR}/InvD_InvDNameTypeIndex.inc' &&

  cat InvD.diss | grep '= Methodref' | awk 'NR==1{print $1}' |
    cut -b 2- > '${INC_FILE_DIR}/InvD_FirstMethodRefIndex.inc' &&
  cat InvD.diss | grep '= InterfaceMethodref' | awk 'NR==1{print $1}' |
    cut -b 2- > '${INC_FILE_DIR}/InvD_FirstInterfaceMethodrefIndex.inc' &&
  cat InvD.diss | grep '= MethodHandle' | awk 'NR==1{print $1}' |
    cut -b 2- > '${INC_FILE_DIR}/InvD_FirstMethodHandleIndex.inc' &&
  cat InvD.diss | grep '= MethodType' | awk 'NR==1{print $1}' |
    cut -b 2- > '${INC_FILE_DIR}/InvD_FirstMethodTypeIndex.inc' &&

  rm -f '${CMAKE_CURRENT_SOURCE_DIR}/InvD.class' InvD.diss"
)

if (OLD_JDK)
  execute_process(
    COMMAND sh -c "echo '-1' > '${INC_FILE_DIR}/Interface_NumInterface.inc'"
  )
elseif (NOT NO_EXTRA_JAVAP)
  target_sources(class_test PUBLIC ClassReaderInterfaceTest.cpp)
  execute_process(
    COMMAND sh -c "javac '${CMAKE_CURRENT_SOURCE_DIR}/Interface.java' &&
    javap -v '${CMAKE_CURRENT_SOURCE_DIR}/C.class' > C.diss &&
    javap -v '${CMAKE_CURRENT_SOURCE_DIR}/Interface.class' | grep = |
      awk 'END{print $1}' | cut -b 2- > '${INC_FILE_DIR}/Interface_NumCPEntries.inc' &&
    cat C.diss | grep interfaces | awk '{print $2}' |
      cut -f 1 -d , > '${INC_FILE_DIR}/Interface_NumInterface.inc' 
    rm -f '${CMAKE_CURRENT_SOURCE_DIR}/Interface.class' '${CMAKE_CURRENT_SOURCE_DIR}/C.class'
    rm -f C.diss Interface.diss"
  )
endif()

if(VERBOSE)
  execute_process(
    COMMAND sh -c "javac '${CMAKE_CURRENT_SOURCE_DIR}/Basic.java' &&
      javap -c -v '${CMAKE_CURRENT_SOURCE_DIR}/Basic.class' &&
      rm '${CMAKE_CURRENT_SOURCE_DIR}/Basic.class'"
  )
endif()

execute_process(
  COMMAND javac "${CMAKE_CURRENT_SOURCE_DIR}/Methods.java"
)

add_test(NAME class_test COMMAND class_test)
