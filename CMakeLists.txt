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

add_subdirectory(deps/SDL-release-2.30.7)

add_executable(rigidBodyBounce src/rigidBody_Bounce.cpp)
target_link_libraries(rigidBodyBounce SDL2main SDL2-static)

add_executable(rigidBodyCollision src/rigidBody_Collision.cpp)
target_link_libraries(rigidBodyCollision SDL2main SDL2-static)

add_executable(SAT_test src/SAT_test.cpp)
target_link_libraries(SAT_test SDL2main SDL2-static)

add_executable(SAT_resolve src/SAT_resolution.cpp)
target_link_libraries(SAT_resolve SDL2main SDL2-static)


add_executable(particleCollision src/particleCollision.cpp)
target_link_libraries(particleCollision SDL2main SDL2-static)

add_executable(rollingWheel src/rollingWheel.cpp)
target_link_libraries(rollingWheel SDL2main SDL2-static)

add_executable(satellite1 src/satellite1.cpp)
target_link_libraries(satellite1 SDL2main SDL2-static)

add_executable(satellite2 src/satellite2.cpp)
target_link_libraries(satellite2 SDL2main SDL2-static)

add_executable(pendulum src/pendulum.cpp)
target_link_libraries(pendulum SDL2main SDL2-static)


add_executable(rigidBody src/rigidBody.cpp)
target_link_libraries(rigidBody SDL2main SDL2-static)

add_executable(inclinedRoll src/inclinedRoll.cpp)
target_link_libraries(inclinedRoll SDL2main SDL2-static)