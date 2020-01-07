
add_library(Class
    ClassFileReader.cpp
    ClassFinder.cpp
    ClassLoader.cpp
)

target_link_libraries(Class zip)
