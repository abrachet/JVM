cmake_minimum_required(VERSION 3.14)

project(JVM LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

set(JVM_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(include)

option(TESTS "Generate build target for unit testing." ON)

add_compile_options("-fno-rtti" "-fno-exceptions" "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

if (TESTS)
    add_definitions(-DTESTING)
endif()

if (NOT DEFINED ENV{JAVA_HOME})
    message(FATAL "JAVA_HOME must be defined!")
else()
    set(JAVA_HOME $ENV{JAVA_HOME})
endif()

execute_process(COMMAND sh -c "find ${JAVA_HOME} -name 'libjava.*'" OUTPUT_VARIABLE LIBJAVA_PATH)
string(STRIP ${LIBJAVA_PATH} LIBJAVA_PATH)

string(REGEX REPLACE "libjava.*" "" LIBJAVA_DIR "${LIBJAVA_PATH}")

execute_process(COMMAND sh -c "find ${JAVA_HOME} -name 'libjvm.*'" OUTPUT_VARIABLE LIBJVM_PATH)
string(STRIP ${LIBJVM_PATH} LIBJVM_PATH)

string(REGEX REPLACE "libjvm.*" "" LIBJVM_DIR "${LIBJVM_PATH}")

set(JVM_INTERNAL_CLASS_DIR ${CMAKE_BINARY_DIR}/classfiles)
execute_process(COMMAND sh -c "mkdir '${JVM_INTERNAL_CLASS_DIR}' 2> /dev/null")

add_subdirectory(lib)
add_subdirectory(src)

if (TESTS)
    add_subdirectory(test)
endif()
