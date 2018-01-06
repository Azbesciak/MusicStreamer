cmake_minimum_required(VERSION 3.9)
project(MusicStreamer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread -std=c++17")

add_executable(MusicStreamer
        utility/json.hpp
        utility/TerminalUtils.h
        server/Server.cpp
        streamerClient/StreamerClient.cpp)