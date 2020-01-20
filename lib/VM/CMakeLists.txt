
add_library(VM
  JNI.cpp
  Stack.cpp
  ThreadContext.cpp
  TypeReader.cpp
)

add_subdirectory(Instructions)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  target_link_options(VM PRIVATE "-rpath" "${JAVA_HOME}/jre/lib/server/" "-rpath" "${JAVA_HOME}/jre/lib/")
endif()
target_link_options(VM PRIVATE -ldl -rpath ${LIBJVM_DIR})

target_compile_definitions(VM PRIVATE LD_SEARCH_PATH=\"${LIBJVM_DIR}:${LIBJAVA_DIR}\")
