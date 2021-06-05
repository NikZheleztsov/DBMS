cmake_minimum_required(VERSION 3.20.1)

project(DBMS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-g")

include_directories (include)
file(GLOB SOURCES "src/*.cpp")

add_executable(DBMS ${SOURCES})
