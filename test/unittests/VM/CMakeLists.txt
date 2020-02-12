
add_executable(vm_test
  AllocatorTest.cpp
  ClassLoaderTest.cpp
  InMemoryObjectTest.cpp
  JNITest.cpp
  ObjectRepresentationTest.cpp
  StackTest.cpp
  ThreadContextTest.cpp
  TypeTest.cpp
)

add_subdirectory(InstructionTests)

include_directories(INCLUDE_DIR)

target_link_libraries(vm_test 
  gtest
  gtest_main 
  VM
)

target_compile_definitions(vm_test PUBLIC 
  LIBJAVA_PATH="${LIBJAVA_PATH}"
  LIBJVM_PATH="${LIBJVM_PATH}")

add_custom_command(
  TARGET vm_test
  POST_BUILD
    COMMAND ${CMAKE_CXX_COMPILER} "${CMAKE_CURRENT_SOURCE_DIR}/ExternSym.cpp" -shared -o
      "$<TARGET_FILE_DIR:vm_test>/ExternSym.so"
)

add_custom_command(
  TARGET vm_test
  POST_BUILD
    COMMAND cd '${CMAKE_CURRENT_SOURCE_DIR}/InstructionTests' &&
    javac '${CMAKE_CURRENT_SOURCE_DIR}/InstructionTests/CallNative.java' &&
    mv '${CMAKE_CURRENT_SOURCE_DIR}/InstructionTests/CallNative.class' '$<TARGET_FILE_DIR:vm_test>/CallNative.class' &&
    mkdir '$<TARGET_FILE_DIR:vm_test>/java' 2> /dev/null || true &&
    mv '${CMAKE_CURRENT_SOURCE_DIR}/InstructionTests/java/PackagedClass.class' '$<TARGET_FILE_DIR:vm_test>/java/PackagedClass.class'
)

add_custom_command(
  TARGET vm_test
  POST_BUILD
    COMMAND javac '${CMAKE_CURRENT_SOURCE_DIR}/InstructionTests/General.java' &&
    mv '${CMAKE_CURRENT_SOURCE_DIR}/InstructionTests/General.class' '$<TARGET_FILE_DIR:vm_test>/General.class'
)

add_custom_command(
  TARGET vm_test
  POST_BUILD
    COMMAND javac '${CMAKE_CURRENT_SOURCE_DIR}/ObjectRepresentation.java'
)

macro(move_file file)
  add_custom_command(
    TARGET vm_test
    POST_BUILD
      COMMAND mv '${CMAKE_CURRENT_SOURCE_DIR}/${file}' '$<TARGET_FILE_DIR:vm_test>/${file}'
  )
endmacro()

move_file("ObjectRepresentationEmpty.class")
move_file("ObjectRepresentationII.class")
move_file("ObjectRepresentationIJ.class")
move_file("ObjectRepresentationJI.class")
move_file("ObjectRepresentationL.class")

add_test(NAME vm_test COMMAND vm_test)
