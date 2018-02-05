#include "TcpServer.h"

TcpServer::TcpServer(const string &host, int port, ServerManager *manager, const string &serverName) : AbstractServer(
        host, port, manager, serverName) {}

int TcpServer::createSocket() {
    return SocketFactory::createTcpSocket(port);
}

void TcpServer::manageServer(sockaddr_in &remote, socklen_t &sockSize) {
    int connection_descriptor = accept(serverFd, (sockaddr *) &remote, &sockSize);
    if (connection_descriptor < 0) {
        perror("Client accepting error");
        return;
    }

    char remoteAddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(remote.sin_addr), remoteAddr, INET_ADDRSTRLEN);
    cout << "Client connected with " << remoteAddr
         << ". Descriptor assigned: " << connection_descriptor << endl;
    thread([&]() {
        cout << "Initialization successfull. Descriptor " << GREEN_TEXT(connection_descriptor) << " from "
             << GREEN_TEXT(remoteAddr) << ".\n";
        onConnection(connection_descriptor, remoteAddr);
    }).detach();
}
