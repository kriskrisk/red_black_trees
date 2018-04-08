cmake_minimum_required(VERSION 3.10)
project(red_black_trees C)

set(CMAKE_C_STANDARD 99)

add_executable(red_black_trees main.c rb_tree.h rb_tree.c)