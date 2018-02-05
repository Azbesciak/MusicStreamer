
#ifndef MUSICSTREAMER_REQUESTRESPONSESERVER_H
#define MUSICSTREAMER_REQUESTRESPONSESERVER_H

#include "TcpServer.h"

class CommunicationServer : public TcpServer {
public:
    CommunicationServer(const string &host, int port, ServerManager *manager);

protected:
    void onNewConnection(int clientSocket, const string &remoteAddr) override;
    void manageRequestCoroutine(int clientSocket, const string &remoteAddr, Request * request, ClientProxy *proxy);
};


#endif //MUSICSTREAMER_REQUESTRESPONSESERVER_H
