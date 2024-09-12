cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include_directories(../server)
include_directories(../client_server_interface)
include_directories(../ManyWorlds/network)

file(GLOB_RECURSE SERVER_SOURCES "../server/*.cpp" "../server/*.hpp" "../server/*.c")
file(GLOB_RECURSE INTERFACE_SOURCES "../client_server_interface/*.hpp")
file(GLOB_RECURSE CLIENT_SOURCES "../ManyWorlds/network/*.hpp")


add_executable(many_worlds_test test.cpp ${SERVER_SOURCES} ${INTERFACE_SOURCES} ${CLIENT_SOURCES})

