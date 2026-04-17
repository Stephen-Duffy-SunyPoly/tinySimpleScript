cmake_minimum_required(VERSION 4.0)
project(tinySimplescript)

set(CMAKE_CXX_STANDARD 20)

add_executable(tinySimplescript main.cpp
        common.hpp
        lcdHighLevel.cpp
        lcdHighLevel.hpp
        commonHighLevel.cpp
        commonHighLevel.hpp)
