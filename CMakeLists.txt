cmake_minimum_required(VERSION 3.0)

project(synthwave)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
add_executable(synthwave src/main.cpp src/Synthesizer.cpp src/Voice.cpp src/Wavetable.cpp src/Wavetable.h)

target_link_libraries(synthwave jack stk)

install(TARGETS synthwave RUNTIME DESTINATION bin)
