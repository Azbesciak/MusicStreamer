#ifndef MUSICSTREAMER_ABSTRACTSERVER_H
#define MUSICSTREAMER_ABSTRACTSERVER_H

#include <iostream>
#include <utility>
#include <netinet/in.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <dirent.h>
#include <iostream>
#include <csignal>


#include "../utility/TerminalUtils.h"
#include "logic/Container.h"
#include "../streamerClient/ClientProxy.h"

#define BUFFER_SIZE 1000
#define QUEUE_SIZE 5


using namespace std;


class AbstractServer {
protected:
    AbstractServer(const string &host, int port, Container *container, atomic<bool> *isRunning);
    virtual ~AbstractServer();

    int port;
    string host;
    thread *serverThread;
    int serverFd;
    Container * container;
    atomic<bool> *isRunning;

    virtual int createSocket() = 0;

    void displayRequest(int socketDescriptor, const char * request);
    void displayResponse(int socketDescriptor, const char *request);
    void manageRequestCoroutine(int clientSocket, const char *remoteAddr, char *buffer,ClientProxy *proxy);
    void onConnection(int clientSocket, const char *remoteAddr);
    void startServer();

};


#endif //MUSICSTREAMER_ABSTRACTSERVER_H
