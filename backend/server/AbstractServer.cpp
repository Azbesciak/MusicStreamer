
#include <unistd.h>
#include "AbstractServer.h"

using namespace std;


AbstractServer::~AbstractServer() {
    close(serverFd);
    delete serverThread;
    cout << MAGENTA_TEXT(this->serverName << " was closed.\n");
}

AbstractServer::AbstractServer(const string &host, int port, ServerManager *manager, const string &serverName)
        : port(port), host(host), manager(manager), serverName(serverName) {
    serverThread = new thread([&]() {

        // Small delay to prevent SIGABRT on calling pure virtual createSocket()
        this_thread::sleep_for(chrono::seconds(1));
        startServer();
    });
    serverThread->detach();
}

void AbstractServer::startServer() {
    cout << GREEN_TEXT(serverName << " works at " <<  host << ":" << port << "\n");

    serverFd = createSocket();
    sockaddr_in remote{};
    socklen_t sockSize = sizeof(sockaddr);
    while (manager->isRunning.load()) {
        manageServer(remote, sockSize);
    }
    exit(0);
}

void AbstractServer::displayRequest(int socketDescriptor, Request *request) {
    cout << YELLOW_TEXT("Client " << socketDescriptor) << "\n";
    cout << "\t"
         << MAGENTA_TEXT("Request from " << socketDescriptor << ":\t")
         << GREEN_TEXT(request->serialize()) << endl;
}


void AbstractServer::displayResponse(int socketDescriptor, const string &response) {
    cout << CYAN_TEXT("Client " << socketDescriptor) << "\n";
    cout << "\t" << MAGENTA_TEXT("Response to " << socketDescriptor << ":\t") << GREEN_TEXT(response) << endl;
}