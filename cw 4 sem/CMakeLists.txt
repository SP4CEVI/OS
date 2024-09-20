cmake_minimum_required(VERSION 3.15)
project (btreemap)

set(CMAKE_CXX_STANDARD 14)

add_executable(main main.cpp database.h btreemap.h interface.h customer.h)