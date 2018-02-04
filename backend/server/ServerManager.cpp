#include "ServerManager.h"

ServerManager::~ServerManager() {
    delete(container);
    closeIfOpened(serverFd);
    closeIfOpened(streamerFd);
    closeIfOpened(broadCastFd);
    closeIfOpened(uploadFd);
}

void ServerManager::closeIfOpened(int fd) {
    if (fd != NOT_INITIALIZED) {
        close(fd);
    }
}
