
#ifndef MUSICSTREAMER_BROADCASTSERVER_H
#define MUSICSTREAMER_BROADCASTSERVER_H


#include "TcpServer.h"

class BroadCastServer : public TcpServer {
public:
    BroadCastServer(const string &host, int port, ServerManager *manager);

protected:
    void onConnection(int clientSocket, const char *remoteAddr) override;

};


#endif //MUSICSTREAMER_BROADCASTSERVER_H
