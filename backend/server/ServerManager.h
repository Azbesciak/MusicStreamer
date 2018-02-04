
#ifndef MUSICSTREAMER_SERVERMANAGER_H
#define MUSICSTREAMER_SERVERMANAGER_H


#define NOT_INITIALIZED (-1)

#include <logic/Container.h>
#include <unistd.h>

class ServerManager {
public:
    int serverFd = NOT_INITIALIZED;
    int broadCastFd = NOT_INITIALIZED;
    int streamerFd = NOT_INITIALIZED;
    int uploadFd = NOT_INITIALIZED;
    Container* container;

    explicit ServerManager(Container* con)
            : container(con){};
    ~ServerManager();

    void closeIfOpened(int fd);
};

#endif //MUSICSTREAMER_SERVERMANAGER_H
