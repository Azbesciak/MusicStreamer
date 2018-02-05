

#ifndef MUSICSTREAMER_UDPSERVER_H
#define MUSICSTREAMER_UDPSERVER_H


#include "AbstractServer.h"

class UdpServer: AbstractServer {
public:
    UdpServer(const string &host, int port, ServerManager *manager, const string &serverName);

private:
    int createSocket() override;
};


#endif //MUSICSTREAMER_UDPSERVER_H
