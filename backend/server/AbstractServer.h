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
#include "ServerManager.h"

#define BUFFER_SIZE 1000
#define QUEUE_SIZE 5

using namespace std;


class AbstractServer {
protected:
    AbstractServer(const string &host, int port, ServerManager *manager, const string &serverName);

    int port;
    string host;
    thread *serverThread;
    int serverFd;
    ServerManager * manager;
    string serverName;

    virtual int createSocket() = 0;

    void displayRequest(int socketDescriptor, const char * request);
    void displayResponse(int socketDescriptor, const char *request);
    void manageRequestCoroutine(int clientSocket, const char *remoteAddr, char *buffer,ClientProxy *proxy);
    void onConnection(int clientSocket, const char *remoteAddr);
    void startServer();

public:
    virtual ~AbstractServer();
};


#endif //MUSICSTREAMER_ABSTRACTSERVER_H
