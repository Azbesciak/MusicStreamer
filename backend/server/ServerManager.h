
#ifndef MUSICSTREAMER_SERVERMANAGER_H
#define MUSICSTREAMER_SERVERMANAGER_H

#include <atomic>

#pragma once
class AbstractServer;
#pragma once
class CommunicationServer;
#pragma once
class Container;

class ServerManager {
public:
    CommunicationServer *communicationServer;
    AbstractServer *broadCaster;
    AbstractServer *streamer;
    AbstractServer *uploader;
    Container* container;
    std::atomic<bool> isRunning;

    explicit ServerManager(Container* con)
            : container(con), isRunning(true),
              communicationServer(nullptr), broadCaster(nullptr),
              streamer(nullptr), uploader(nullptr){};

};


#endif //MUSICSTREAMER_SERVERMANAGER_H
