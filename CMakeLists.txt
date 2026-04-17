cmake_minimum_required(VERSION 4.0)
project(tinySimplescript)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        cpp-subprocess
        GIT_REPOSITORY https://github.com/arun11299/cpp-subprocess.git
        GIT_TAG        v2.5
)

FetchContent_MakeAvailable(cpp-subprocess)


add_executable(tinySimplescript main.cpp
        common.hpp
        lcdHighLevel.cpp
        lcdHighLevel.hpp
        commonHighLevel.cpp
        commonHighLevel.hpp
        common.cpp)

target_link_libraries(tinySimplescript PRIVATE cpp-subprocess::subprocess)
