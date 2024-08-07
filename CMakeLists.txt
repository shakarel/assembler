cmake_minimum_required(VERSION 3.28)
project(assembler C)

set(CMAKE_C_STANDARD 11)

add_executable(assembler src/main.c
        include/macros.h
        src/macros.c
        src/preprocessor.c
        include/preprocessor.h
        src/helper_preprocessor_functions.c
        include/ast.h
        src/frontend.c
)
