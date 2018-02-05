#include "RequestResponseServer.h"

RequestResponseServer::RequestResponseServer(
        const string &host, int port, ServerManager *manager)
        : AbstractServer(host, port, manager, "Request-Response server") {
}


int RequestResponseServer::createSocket() {
    return SocketFactory::createTcpSocket(port);
}
