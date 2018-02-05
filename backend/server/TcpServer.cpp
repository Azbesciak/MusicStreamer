#include "TcpServer.h"

TcpServer::TcpServer(const string &host, int port, ServerManager *manager, const string &serverName) : AbstractServer(
        host, port, manager, serverName) {}

int TcpServer::createSocket() {
    return SocketFactory::createTcpSocket(host, port);
}

void TcpServer::manageServer(sockaddr_in &remote, socklen_t &sockSize) {
    int clientSocket = accept(serverFd, (sockaddr *) &remote, &sockSize);
    if (clientSocket < 0) {
        perror("Client accepting error");
        return;
    }

    char remoteAddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(remote.sin_addr), remoteAddr, INET_ADDRSTRLEN);
    cout << "Client connected with " << remoteAddr
         << ". Descriptor assigned: " << clientSocket << endl;
    thread([&]() {
        onNewConnection(clientSocket, remoteAddr);
    }).detach();
}
