cmake_minimum_required(VERSION 3.10)
project(TinySocketExample)

set(CMAKE_CXX_STANDARD 17)

add_executable(server src/server.cpp)
add_executable(client src/client.cpp)