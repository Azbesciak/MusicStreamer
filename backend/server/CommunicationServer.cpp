#include "CommunicationServer.h"

CommunicationServer::CommunicationServer(
        const string &host, int port, ServerManager *manager)
        : TcpServer(host, port, manager, "Communication server") {
}

void CommunicationServer::onNewConnection(int clientSocket, const string &remoteAddr) {
    thread([clientSocket, remoteAddr, this]() {
        cout << "Initialization successfull. Descriptor "
             << BLUE_TEXT(clientSocket) << " from "
             << BLUE_TEXT(remoteAddr) << endl;
        auto buffer = new char[BUFFER_SIZE];
        auto proxy = new ClientProxy(clientSocket, this->manager->container);
        while (true) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t value = read(clientSocket, buffer, BUFFER_SIZE);
            if (value > 0) {
                manageRequestCoroutine(clientSocket, remoteAddr, buffer, proxy);
            } else if (buffer[0] == 0) {
                cout << RED_TEXT("Client from " << remoteAddr
                                                << ", descriptor "
                                                << clientSocket
                                                << " has disconnected ("
                                                << this->serverName
                                                << ")!\n");
                delete proxy;
                return;
            } else {
                cout << "Undefined response." << endl;
            }
        }
    }).detach();
}

void CommunicationServer::manageRequestCoroutine(int clientSocket, const string &remoteAddr, char *buffer, ClientProxy *proxy) {
    displayRequest(clientSocket, buffer);
    auto response = proxy->onNewMessage(buffer);
    auto serializedResponse = response.serialize();
    displayResponse(clientSocket, serializedResponse.c_str());
    const auto wrote = proxy->sendMessage(serializedResponse);
    if (wrote == -1) {
        cout << RED_TEXT("Error while sending client response: \n\t")
             << MAGENTA_TEXT(serializedResponse)
             << RED_TEXT("\n to " << remoteAddr);
    }
}