# Set CMake version
cmake_minimum_required(VERSION 3.17)

# Set project details
project(PacMan LANGUAGES CXX VERSION 1.0.0 DESCRIPTION "Classic Pac-man clone using C++17 and IME")

# Set CXX version used by the project
set(CMAKE_CXX_STANDARD 17)

#Build game
add_subdirectory(src)