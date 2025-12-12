cmake_minimum_required(VERSION 4.0)

if(WIN32)
    set(CMAKE_C_COMPILER clang-cl)
    set(CMAKE_CXX_COMPILER clang-cl)
elseif(APPLE)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
endif()

project(cpp_snake_game)

set(CMAKE_CXX_STANDARD 14)

add_executable(cpp_snake_game
        main.cpp)
