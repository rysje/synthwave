cmake_minimum_required(VERSION 3.0)

project(synthwave)

add_executable(synthwave src/main.cpp src/Synthesizer.cpp src/Voice.cpp src/Wavetable.cpp src/Wavetable.h)

target_link_libraries(synthwave jack stk)

install(TARGETS synthwave RUNTIME DESTINATION bin)
