
#ifndef MUSICSTREAMER_BROADCASTSERVER_H
#define MUSICSTREAMER_BROADCASTSERVER_H


#include "TcpServer.h"

class BroadCastRequestProcessor : public RequestProcessor {
    ClientResponse onNewRequest(Request *request, const string &method, ClientResponse *&response) override;
    ServerManager *manager;
    int clientSocket;
public :
    BroadCastRequestProcessor(ServerManager *manager, int clientSocket);
    ClientResponse onNewRequest(Request *request) override;
    ssize_t respond(ClientResponse * response);
};

class BroadCastServer : public TcpServer {
public:
    BroadCastServer(const string &host, int port, ServerManager *manager);

protected:
    void onNewConnection(int clientSocket, const string &remoteAddr) override;

};


#endif //MUSICSTREAMER_BROADCASTSERVER_H
