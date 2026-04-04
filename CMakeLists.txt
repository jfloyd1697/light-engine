cmake_minimum_required(VERSION 3.20)
project(light_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LIGHT_BUILD_SIM "Build the PC simulator" ON)

add_subdirectory(lib/light_core)

if(LIGHT_BUILD_SIM)
    add_subdirectory(tools/light_sim)
endif()
