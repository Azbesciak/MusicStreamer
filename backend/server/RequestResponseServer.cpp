#include "RequestResponseServer.h"

RequestResponseServer::RequestResponseServer(
        const string &host, int port, ServerManager *manager)
        : TcpServer(host, port, manager, "Request-Response server") {
}

