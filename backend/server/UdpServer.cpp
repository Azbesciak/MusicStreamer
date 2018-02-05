#include "UdpServer.h"

UdpServer::UdpServer(const string &host, int port, ServerManager *manager, const string &serverName) : AbstractServer(
        host, port, manager, serverName) {}
