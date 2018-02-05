#include "CommunicationServer.h"
#include "RequestReader.h"

CommunicationServer::CommunicationServer(
        const string &host, int port, ServerManager *manager)
        : TcpServer(host, port, manager, "Communication server") {
}

void CommunicationServer::onNewConnection(int clientSocket, const string &remoteAddr) {
    thread([clientSocket, remoteAddr, this]() {
        cout << "Initialization successfull. Descriptor "
             << BLUE_TEXT(clientSocket) << " from "
             << BLUE_TEXT(remoteAddr) << endl;
        auto reader = new RequestReader(clientSocket);
        auto proxy = new ClientProxy(clientSocket, this->manager->container);
        while (true) {
            Request *request = reader->readRequest();
            if (request != nullptr) {
                manageRequestCoroutine(clientSocket, remoteAddr, request, proxy);
                delete request;
            } else {
                cout << RED_TEXT("Client from " << remoteAddr
                                                << ", descriptor "
                                                << clientSocket
                                                << " has disconnected ("
                                                << this->serverName
                                                << ")!\n");
                delete proxy;
                return;
            }
        }
    }).detach();
}

void CommunicationServer::manageRequestCoroutine(int clientSocket, const string &remoteAddr, Request * request, ClientProxy *proxy) {
    displayRequest(clientSocket, request);
    auto response = proxy->onNewMessage(request);
    auto serializedResponse = response.serialize();
    displayResponse(clientSocket, serializedResponse);
    const auto wrote = proxy->sendMessage(serializedResponse);
    if (wrote == -1) {
        cout << RED_TEXT("Error while sending client response: \n\t")
             << MAGENTA_TEXT(serializedResponse)
             << RED_TEXT("\n to " << remoteAddr);
    }
}