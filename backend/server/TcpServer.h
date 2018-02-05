

#ifndef MUSICSTREAMER_TCPSERVER_H
#define MUSICSTREAMER_TCPSERVER_H


#include "AbstractServer.h"
#include "SocketFactory.h"

class TcpServer : public AbstractServer {
public:
    TcpServer(const string &host, int port, ServerManager *manager, const string &serverName);

protected:
    int createSocket() override;
};


#endif //MUSICSTREAMER_TCPSERVER_H
