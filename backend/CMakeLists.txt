cmake_minimum_required(VERSION 3.9)
project(MusicStreamer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread -std=c++17")

include_directories(.)

add_executable(MusicStreamer
        utility/json.hpp
        utility/TerminalUtils.h
        server/ServerRunner.cpp
        streamerClient/StreamerClient.cpp
        logic/MusicStreamer.cpp
        logic/MusicStreamer.h
        logic/Room.cpp
        logic/Room.h
        logic/Container.cpp
        logic/Container.h
        logic/TracksQueue.cpp
        logic/TracksQueue.h
        utility/synch.h
        streamerClient/ClientResponse.cpp
        streamerClient/ClientResponse.h
        streamerClient/ClientProxy.cpp
        streamerClient/ClientProxy.h
        messageSender/Message.h
        messageSender/Message.cpp
        messageSender/MessageSender.cpp
        messageSender/MessageSender.h
        upload/FileUpload.cpp
        upload/FileUpload.h
        upload/UploadHandler.cpp
        upload/UploadHandler.h
        server/SocketFactory.cpp
        server/SocketFactory.h
        logic/MusicTrack.cpp
        logic/MusicTrack.h
        utility/token.hpp
        upload/TrackUpload.cpp
        upload/TrackUpload.h
        upload/UploadMeta.cpp
        upload/UploadMeta.h
        upload/UploadedFile.cpp
        upload/UploadedFile.h
        upload/exception/FileUploadException.cpp
        upload/exception/FileUploadException.h
        utility/Config.cpp
        utility/Config.h
        server/ServerManager.h
        server/CommunicationServer.cpp
        server/CommunicationServer.h
        server/AbstractServer.cpp
        server/AbstractServer.h
        server/TcpServer.cpp
        server/TcpServer.h
        server/BroadCastServer.cpp
        server/BroadCastServer.h
        streamerClient/Socket.h
        streamerClient/Request.cpp
        streamerClient/Request.h
        server/RequestReader.cpp
        server/RequestReader.h
        streamerClient/RequestProcessor.cpp
        streamerClient/RequestProcessor.h
        server/music/MusicChannel.cpp
        server/music/MusicChannel.h
        server/UdpServer.cpp
        server/UdpServer.h
        logic/TrackStream.cpp logic/TrackStream.h logic/callback/OnTrackQueuedListener.h)
