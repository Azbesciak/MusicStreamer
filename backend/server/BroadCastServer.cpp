
#include "BroadCastServer.h"

BroadCastServer::BroadCastServer(const string &host, int port, ServerManager *manager)
        : TcpServer(host, port, manager, "BroadCast server") {}

void BroadCastServer::onConnection(int clientSocket, const char *remoteAddr) {

}
