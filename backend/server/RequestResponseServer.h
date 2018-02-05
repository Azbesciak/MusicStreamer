
#ifndef MUSICSTREAMER_REQUESTRESPONSESERVER_H
#define MUSICSTREAMER_REQUESTRESPONSESERVER_H

#include "TcpServer.h"

class RequestResponseServer : public TcpServer {
public:
    RequestResponseServer(const string &host, int port, ServerManager *manager);

protected:
    void onConnection(int clientSocket, const char *remoteAddr) override;
    void manageRequestCoroutine(int clientSocket, const char *remoteAddr, char *buffer, ClientProxy *proxy);
};


#endif //MUSICSTREAMER_REQUESTRESPONSESERVER_H
