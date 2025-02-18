cmake_minimum_required(VERSION 3.10)
project(TinySocketExample)

set(CMAKE_CXX_STANDARD 17)

add_executable(server src/server.cpp)
add_executable(client src/client.cpp)

add_executable(udp_server src/udp_server.cpp)
add_executable(udp_client src/udp_client.cpp)