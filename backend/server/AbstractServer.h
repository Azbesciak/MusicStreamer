#ifndef MUSICSTREAMER_ABSTRACTSERVER_H
#define MUSICSTREAMER_ABSTRACTSERVER_H

#include <iostream>
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../utility/TerminalUtils.h"
#include "../streamerClient/ClientProxy.h"
#include "ServerManager.h"


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
    virtual void manageServer(sockaddr_in & remote, socklen_t & sockSize) = 0;

    void displayRequest(int socketDescriptor, Request * request);
    void displayResponse(int socketDescriptor,const string &response);
    void startServer();

public:
    virtual ~AbstractServer();
};


#endif //MUSICSTREAMER_ABSTRACTSERVER_H
