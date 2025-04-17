cmake_minimum_required(VERSION 3.28)
project(zad_4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static -W")

add_executable(zad_4 mainProgram/main.cpp
        Graph/makeGraph.cpp
        Graph/makeGraph.h
        mainProgram/Program.cpp
        mainProgram/Program.h
        mainProgram/Timer.cpp
        mainProgram/Timer.h Algorithms/Ants.cpp Algorithms/Ants.h)
