cmake_minimum_required(VERSION 3.14)

project(JVM LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

option(TESTS "Generate build target for unit testing." ON)

add_compile_options("-fno-rtti" "-fno-exceptions" "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

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

add_subdirectory(src)
add_subdirectory(lib)

if (TESTS)
    add_subdirectory(test)
endif()
