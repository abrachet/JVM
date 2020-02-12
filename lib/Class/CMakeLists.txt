
add_library(Class
    ClassFile.cpp
    ClassFileReader.cpp
    ClassFinder.cpp
)

target_link_libraries(Class Core)

find_library(ZIP zip)
# TODO: Find zip.h better.
include_directories("/usr/local/include")
target_link_libraries(Class ${ZIP})
