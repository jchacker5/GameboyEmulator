cmake_minimum_required(VERSION 3.20)
project(GameboyEmulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(gameboy src/gameboy.cpp src/screen.cpp src/screen.h src/Z80.cpp src/Z80.h)
target_link_libraries(gameboy PRIVATE Qt6::Widgets)
