
#include "BroadCastServer.h"
#include "RequestReader.h"

BroadCastServer::BroadCastServer(const string &host, int port, ServerManager *manager)
        : TcpServer(host, port, manager, "BroadCast server") {}

void BroadCastServer::onNewConnection(int clientSocket, const string &remoteAddr) {
    auto reader = new RequestReader(clientSocket);
    auto request = reader->readRequest();
    if (request != nullptr) {
        auto processor = new BroadCastRequestProcessor(manager, clientSocket);
        auto response = processor->onNewRequest(request);
        processor->respond(&response);
        if (response.isError()) {
            close(clientSocket);
        }
        delete processor;
        delete request;
    } else {
        close(clientSocket);
    }

}

ClientResponse BroadCastRequestProcessor::onNewRequest(Request *request, const string &method, ClientResponse *&response) {
    if (request->getMethod() == "SUBSCRIBE") {
        auto name = request->getStr("name");
        auto client = manager->container->subscribeClientForMessages(name, clientSocket);
        if (client != nullptr) {
            cout<< MAGENTA_TEXT("CLIENT " << name << "(fd " << clientSocket << ") subscribed for messages") <<endl;
        } else {
            response->setError(404, "User not found. Connection will be closed");
        }
    }
    return *response;
}

BroadCastRequestProcessor::BroadCastRequestProcessor(ServerManager *manager, int clientSocket)
        : manager(manager), clientSocket(clientSocket) {}

ClientResponse BroadCastRequestProcessor::onNewRequest(Request *request) {
    return RequestProcessor::onNewRequest(request);
}

ssize_t BroadCastRequestProcessor::respond(ClientResponse *response) {
    return RequestProcessor::respond(clientSocket, response->serialize());
}
