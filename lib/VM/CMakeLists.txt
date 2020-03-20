
add_library(VM
  Allocator.cpp
  ClassHierarchyWalker.cpp
  ClassLoader.cpp
  JNI.cpp
  ObjectRepresentation.cpp
  Stack.cpp
  ThreadContext.cpp
  Type.cpp
)

target_link_libraries(VM Class)

add_subdirectory(Instructions)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  target_link_options(VM PRIVATE "-rpath" "${JAVA_HOME}/jre/lib/server/" "-rpath" "${JAVA_HOME}/jre/lib/")
endif()
target_link_options(VM PRIVATE -ldl -rpath ${LIBJVM_DIR})

target_compile_definitions(VM PRIVATE
  LD_SEARCH_PATH=\"${LIBJVM_DIR}:${LIBJAVA_DIR}\"
  JVM_BUILTIN_CLASS_PATH=\"${JVM_INTERNAL_CLASS_DIR}\"
)
