cmake_minimum_required(VERSION 3.0)

project(synthwave)

add_executable(synthwave src/main.cpp src/Synthesizer.cpp src/Voice.cpp)

target_link_libraries(synthwave jack)

install(TARGETS synthwave RUNTIME DESTINATION bin)
