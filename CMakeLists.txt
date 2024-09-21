cmake_minimum_required(VERSION 3.19)
project(Physics_for_cpp_games_animation VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER em++)
endif()

include_directories(include)

add_subdirectory(src/rigidBodyTest)
