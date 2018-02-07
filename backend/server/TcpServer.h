

#ifndef MUSICSTREAMER_TCPSERVER_H
#define MUSICSTREAMER_TCPSERVER_H


#include "AbstractServer.h"
#include "SocketFactory.h"


class TcpServer : public AbstractServer {
public:
    TcpServer(const std::string &host, int port, ServerManager *manager, const std::string &serverName);

protected:
    int createSocket() override;

    void manageServer(sockaddr_in &remote, socklen_t &sockSize) override;
    virtual void onNewConnection(int clientSocket, const std::string &remoteAddr) = 0;
};


#endif //MUSICSTREAMER_TCPSERVER_H
