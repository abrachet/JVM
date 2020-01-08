
add_library(Class
    ClassFileReader.cpp
    ClassFinder.cpp
    ClassLoader.cpp
)

find_library(ZIP zip)
# TODO: Find zip.h better.
include_directories("/usr/local/include")
target_link_libraries(Class ${ZIP})
