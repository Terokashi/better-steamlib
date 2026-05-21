cmake_minimum_required(VERSION 3.16)

project(better_steamlib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional but recommended for Qt projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(core)
add_subdirectory(cli)
add_subdirectory(gui)