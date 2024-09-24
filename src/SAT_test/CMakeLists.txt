add_executable(SAT_test main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

if(EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_FUNCTIONS='[_sat_main, _setSize]'")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_RUNTIME_METHODS='[ccall, cwrap]'")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sUSE_SDL=2 -v")

    set_target_properties(SAT_test PROPERTIES 
    OUTPUT_NAME "../../pages/SAT_test/main" SUFFIX ".js")
endif()

# target_link_libraries(rigidBodyTest SDL2main SDL2-static)
# 
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ../assets")
