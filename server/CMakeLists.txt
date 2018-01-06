cmake_minimum_required(VERSION 3.9)
project(MusicStreamer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread -Wtraditional")

add_executable(MusicStreamer json.hpp utility/terminal.h Server.cpp)