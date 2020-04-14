cmake_minimum_required(VERSION 3.7)

project(Othello)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -O2)
set(CMAKE_EXE_LINKER_FLAGS -O2)
set(SOURCE_FILES Board.h MyStrategy.h Runner.h Strategy.h othello.cpp)
add_executable(othello ${SOURCE_FILES})