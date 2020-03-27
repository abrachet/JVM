
add_executable(JVM
  jvm.cpp
)
target_link_libraries(JVM
  Class
  Core
  VM
)

# TODO: In the future we will want to use std::embed
add_custom_command(
  TARGET JVM
  DEPENDS Start.java
  COMMAND javac ${CMAKE_CURRENT_SOURCE_DIR}/Start.java && 
          mv ${CMAKE_CURRENT_SOURCE_DIR}/__JVM_internal_Start.class ${JVM_INTERNAL_CLASS_DIR}/
  BYPRODUCTS ${JVM_INTERNAL_CLASS_DIR}/__JVM_internal_Start.class
)
