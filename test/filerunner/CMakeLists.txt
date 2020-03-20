
set(FILERUNNER ${CMAKE_BINARY_DIR}/bin/filerunner)
add_custom_command(
  OUTPUT ${FILERUNNER}
  DEPENDS ${JVM_SRC_DIR}/utils/filerunner.go
  COMMAND go build ${JVM_SRC_DIR}/utils/filerunner.go && mv filerunner ${FILERUNNER}
)

set(filerunner_srcs
  Add.java
  Arrays.java
  CallNative.java
  ClassFields.java
  DoNothing.java
  FilerunnerFailure.java
  FilerunnerTest.java
  InstanceOf.java
  IntMain.java
  Loop.java
  Method.java
  NoInputFiles.txt
  Virtual.java
)

add_custom_target(filerunner_test
  DEPENDS ${FILERUNNER} JVM Natives.java
)

foreach(file ${filerunner_srcs})
  add_custom_command(
    TARGET filerunner_test
    COMMAND ${FILERUNNER} -DJVM=${CMAKE_BINARY_DIR}/src/JVM ${CMAKE_CURRENT_SOURCE_DIR}/${file}
    COMMAND rm -f ${CMAKE_CURRENT_SOURCE_DIR}/*.class
  )
endforeach()
