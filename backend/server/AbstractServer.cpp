
#include <unistd.h>
#include "AbstractServer.h"

AbstractServer::~AbstractServer() {
    close(serverFd);
    delete serverThread;
    cout << MAGENTA_TEXT(this->serverName << " was closed.\n");
}

AbstractServer::AbstractServer(const string &host, int port, ServerManager *manager, const string &serverName)
        : port(port), host(host), manager(manager), serverName(serverName) {
    this->serverThread = new thread([&]() {
        startServer();
    });
    this->serverThread->detach();
}

void AbstractServer::startServer() {
    cout << GREEN_TEXT(this->serverName << " works at " <<  this->host << ":" << this->port << "\n");

    serverFd = createSocket();
    sockaddr_in remote{};
    socklen_t sockSize = sizeof(sockaddr);
    while (this->manager->isRunning.load()) {
        manageServer(remote, sockSize);
    }
    exit(0);
}

void AbstractServer::displayRequest(int socketDescriptor, const char *request) {
    cout << YELLOW_TEXT("Client " << socketDescriptor) << "\n";
    cout << "\t" << MAGENTA_TEXT("Request from " << socketDescriptor << ":\t") << GREEN_TEXT(request);
}


void AbstractServer::displayResponse(int socketDescriptor, const char *response) {
    cout << CYAN_TEXT("Client " << socketDescriptor) << "\n";
    cout << "\t" << MAGENTA_TEXT("Response to " << socketDescriptor << ":\t") << GREEN_TEXT(response) << endl;
}