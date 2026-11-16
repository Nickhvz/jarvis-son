cmake_minimum_required(VERSION 3.16)
project(jarvis-son LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(jarvis-son
    src/main.cpp
    src/orchestrator.cpp
)
