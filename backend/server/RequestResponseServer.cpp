#include "RequestResponseServer.h"

RequestResponseServer::~RequestResponseServer() {

}


RequestResponseServer::RequestResponseServer(
        const string &host, int port, Container *container, atomic<bool> *isRunning)
        : AbstractServer(host, port, container, isRunning) {
}


int RequestResponseServer::createSocket() {
    return SocketFactory::createTcpSocket(port);
}
