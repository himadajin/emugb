cmake_minimum_required(VERSION 3.16)

project(emugb CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(emugb
    src/cartridge.cpp
    src/cpu.cpp
    src/main.cpp
    src/memory.cpp
    src/mmu.cpp
)
