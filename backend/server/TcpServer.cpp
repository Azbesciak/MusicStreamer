#include "TcpServer.h"

TcpServer::TcpServer(const string &host, int port, ServerManager *manager, const string &serverName) : AbstractServer(
        host, port, manager, serverName) {}

int TcpServer::createSocket() {
    return SocketFactory::createTcpSocket(port);
}
