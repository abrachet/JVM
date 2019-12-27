
add_library(Class
    ClassFileReader.cpp
    ClassFinder.cpp
)

target_link_libraries(Class zip)
