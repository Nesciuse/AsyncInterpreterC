cmake_minimum_required(VERSION 3.23)
project(test)

include_directories(
    C:/include/glib-2.0
    c:/lib/glib-2.0/include/
)

add_executable(simple
    interpreter.c
    map.c
)

target_link_directories(simple
    PRIVATE c:/lib
)

target_link_libraries(simple
    glib-2.0
    intl
)