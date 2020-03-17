
add_custom_target(filerunner
  COMMAND go build ${JVM_SRC_DIR}/utils/filerunner.go
)

set(filerunner_srcs
  Add.java
  CallNative.java
  DoNothing.java
  FilerunnerFailure.java
  FilerunnerTest.java
  NoInputFiles.txt
)

add_custom_target(filerunner_test
  DEPENDS filerunner JVM Natives.java
)

foreach(file ${filerunner_srcs})
  add_custom_command(
    TARGET filerunner_test
    COMMAND ${JVM_SRC_DIR}/utils/filerunner -DJVM=${CMAKE_BINARY_DIR}/src/JVM ${CMAKE_CURRENT_SOURCE_DIR}/${file}
    COMMAND rm -f ${CMAKE_CURRENT_SOURCE_DIR}/*.class
  )
endforeach()
