cmake_minimum_required(VERSION 3.0)

project(synthwave)

add_executable(synthwave main.cpp)

install(TARGETS synthwave RUNTIME DESTINATION bin)
