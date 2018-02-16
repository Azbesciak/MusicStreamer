
#include <unistd.h>
#include "BroadCastServer.h"
#include "RequestReader.h"

BroadCastServer::BroadCastServer(const string &host, int port, ServerManager *manager)
        : TcpServer(host, port, manager, "BroadCast server") {}

void BroadCastServer::onNewConnection(int clientSocket, const string &remoteAddr) {
    auto reader = new RequestReader(clientSocket);
    auto processor = new BroadCastRequestProcessor(manager, clientSocket);
    bool shouldStop = false;
    int tries = 3;
    while (tries > 0 && !shouldStop) {
        auto requests = reader->readRequest();
        for (auto request : requests) {
            tries--;
            if (request != nullptr) {
                auto response = processor->onNewRequest(request);
                if (response.isError()) {
                    response.addToBody("tries", tries);
                }
                processor->respond(&response);
                if (!response.isError()) {
                    shouldStop = true;
                    break;
                }
            }
            if (request == nullptr || tries <= 0) {
                cout << "Socket " << clientSocket << " was closed." << endl;
                close(clientSocket);
                tries = -1;
            }
        }
        for (auto req: requests) {
           delete req;
        }
    }
    delete processor;
    delete reader;
}

ClientResponse BroadCastRequestProcessor::onNewRequest(Request *request, const string &method, ClientResponse *&response) {
    if (request->getMethod() == "SUBSCRIBE") {
        auto name = request->getStr("name");
        auto client = Container::getInstance()->subscribeClientForMessages(name, clientSocket);
        if (client != nullptr) {
            cout<< MAGENTA_TEXT("CLIENT " << name << "(fd " << clientSocket << ") subscribed for messages") <<endl;
        } else {
            response->setError(404, "User not found.");
        }
    } else {
        response->asUnknownResponse();
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
