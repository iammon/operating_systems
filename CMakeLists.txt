cmake_minimum_required(VERSION 3.10)
project(MyProject C)

set(CMAKE_C_STANDARD 11)

add_executable(linuxProc linuxProc.c)

add_custom_target(run
    COMMAND ./linuxProc
    DEPENDS linuxProc
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
