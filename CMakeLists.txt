cmake_minimum_required(VERSION 3.14)

project(JVM LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

option(TESTS "Generate build target for unit testing." ON)

add_compile_options("-fno-rtti" "-fno-exceptions" "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

if (TESTS)
    add_subdirectory(test)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
add_subdirectory(lib)
