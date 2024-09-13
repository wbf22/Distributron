cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include_directories(./distributron)

file(GLOB_RECURSE SOURCE "./distributron/*.cpp" "./distributron/*.hpp" "./distributron/*.c")


add_executable(distributron ./distributron/distributron.cpp ${SOURCE})

