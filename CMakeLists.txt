cmake_minimum_required(VERSION 3.5.0)
project(Physics_for_cpp_games_animation_with_opengl VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(deps/SDL-release-2.30.7)

include_directories(deps/SDL-release-2.30.7/include)

file(GLOB SRC_FILES "src/*.cpp")

foreach (SRC IN LISTS SRC_FILES)
    string(REGEX MATCH "[A-Z|a-z|0-9]+.cpp" FILE_NAME ${SRC})
    string(REPLACE ".cpp" "" FILE ${FILE_NAME})
    add_executable("phy_${FILE}" "src/${FILE_NAME}")
    target_link_libraries("phy_${FILE}" SDL2main SDL2-static)
endforeach()