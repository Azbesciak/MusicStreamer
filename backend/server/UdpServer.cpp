#include "UdpServer.h"
#include "SocketFactory.h"

UdpServer::UdpServer(const string &host, int port, ServerManager *manager, const string &serverName) : AbstractServer(
        host, port, manager, serverName) {}

int UdpServer::createSocket() {
    return SocketFactory::createUdpSocket(port);
}
