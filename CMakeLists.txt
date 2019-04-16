cmake_minimum_required(VERSION 2.8)

project(JsonRpcServer)

add_executable(JsonRpcServer main.cpp JsonRpc.cpp JsonRpcServer.cpp)

set(CMAKE_CXX_FLAGS "-g -Wdeprecated-declarations -std=c++11")

target_link_libraries(JsonRpcServer json)
target_link_libraries(JsonRpcServer pthread)
