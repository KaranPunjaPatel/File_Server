cmake_minimum_required(VERSION 3.10)
project(File_Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories
add_subdirectory(src/Security)
add_subdirectory(src/Network)
add_subdirectory(src/Graphics)
add_subdirectory(src/Client)
add_subdirectory(src/Server)
