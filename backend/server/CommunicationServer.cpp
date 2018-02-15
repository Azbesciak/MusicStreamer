#include "CommunicationServer.h"
#include "RequestReader.h"

CommunicationServer::CommunicationServer(
        const string &host, int port, ServerManager *manager)
        : TcpServer(host, port, manager, "Communication server") {
}

void CommunicationServer::onNewConnection(int clientSocket, const string &remoteAddr) {
    cout << "Initialization successful. Descriptor "
         << BLUE_TEXT(clientSocket) << " from "
         << BLUE_TEXT(remoteAddr) << endl;
    auto *reader = new RequestReader(clientSocket);
    auto proxy = new ClientProxy(clientSocket, remoteAddr, manager->container);
    bool hasDisconnected = false;
    while (!hasDisconnected) {
        auto requests = reader->readRequest();
        for (auto req : requests) {
            auto wasSuccessful = false;
            if (req != nullptr) {
                wasSuccessful = manageRequest(clientSocket, remoteAddr, req, proxy);
            } else {
                cout << RED_TEXT("Client from " << remoteAddr << ", descriptor " << clientSocket
                                                << " has disconnected (" << serverName << ")!\n");
            }
            if (!wasSuccessful) {
                cout << BLUE_TEXT("Closing connection with " << clientSocket) << endl;
                delete proxy;
                hasDisconnected = true;
                break;
            }
        }
        for (auto req: requests) {
            delete req;
        }
    }
}

bool CommunicationServer::manageRequest(int clientSocket, const string &remoteAddr, Request *request,
                                                 ClientProxy *proxy) {
    displayRequest(clientSocket, request);
    auto response = proxy->onNewRequest(request);
    auto serializedResponse = response.serialize();
    displayResponse(clientSocket, serializedResponse);
    bool wrote = proxy->respond(serializedResponse) >= 0;
    if (!wrote) {
        cout << RED_TEXT("Error while sending client response: \n\t")
             << MAGENTA_TEXT(serializedResponse)
             << RED_TEXT("\n to " << remoteAddr)
             << endl;
    }
    return wrote;
}