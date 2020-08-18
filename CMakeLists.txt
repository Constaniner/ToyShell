cmake_minimum_required(VERSION 3.14)
project(CA_Project4 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(CA_Project4
        parse.c
        parse.h
        shell.c)
