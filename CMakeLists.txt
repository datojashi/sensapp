cmake_minimum_required(VERSION 3.5)

project(sensapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sensapp main.cpp)

target_include_directories(sensapp PRIVATE ${CMAKE_SOURCE_DIR}, ../AWL)
target_link_libraries(sensapp pthread)
