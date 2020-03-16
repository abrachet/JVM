
add_custom_target(filerunner
  COMMAND go build ${JVM_SRC_DIR}/utils/filerunner.go
)

set(filerunner_srcs
  FilerunnerFailure.java
  FilerunnerTest.java
)

add_custom_target(filerunner_test)

foreach(file ${filerunner_srcs})
  add_custom_command(
    TARGET filerunner_test
    DEPENDS filerunner
    COMMAND ${JVM_SRC_DIR}/utils/filerunner ${CMAKE_CURRENT_SOURCE_DIR}/${file}
  )
endforeach()
