cmake_minimum_required(VERSION 3.9)
project(MusicStreamer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread -std=c++17")

add_executable(MusicStreamer
        utility/json.hpp
        utility/TerminalUtils.h
        server/ServerRunner.cpp
        streamerClient/StreamerClient.cpp
        streamerClient/MusicStreamer.cpp
        streamerClient/MusicStreamer.h
        streamerClient/Room.cpp
        streamerClient/Room.h
        server/Container.cpp
        server/Container.h
        utility/synch.h streamerClient/ClientResponse.cpp streamerClient/ClientResponse.h)