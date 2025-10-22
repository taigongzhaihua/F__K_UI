cmake_minimum_required(VERSION 3.10)
project(FK_UI_Minimal VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(fk_example
    examples/main.cpp
    src/Application.cpp
    src/Window.cpp
)

target_include_directories(fk_example PRIVATE ${CMAKE_SOURCE_DIR}/src)
