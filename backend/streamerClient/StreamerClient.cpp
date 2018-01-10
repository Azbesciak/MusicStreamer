
#include <unistd.h>
#include "../utility/json.hpp"
#include "StreamerClient.h"

using json = nlohmann::json;
using namespace std;

StreamerClient::StreamerClient(int socketDescriptor, Container *container) :
        socketDescriptor(socketDescriptor),
        container(container) {}

ClientResponse StreamerClient::onNewMessage(char *message) {
    ClientResponse resp;
    try {
        auto request = json::parse(message);
        auto method = request.at(METHOD_KEY).get<string>();
        if (method == AUTHENTICATE_ACTION) {
            return authenticate(method, request);
        } else if (name.empty()) {
            return ClientResponse::error(403, "Not authenticated");
        } else if (method == JOIN_ROOM_ACTION) {
            auto roomName = request.at("roomName").get<string>();
            container->joinClientToRoom(this, roomName);
        } else if (method == GET_ROOMS_ACTION) {
            resp.addToBody("rooms", container->getRoomsList());
        }
        resp.fillOkResultIfNotSet();

    } catch (json::exception &e) {
        cout << e.what() << endl;
        resp.setError(500, "Malformed input data");
    }
    return resp;
}

ClientResponse StreamerClient::authenticate(const string &method, json request) {
    ClientResponse resp;
    if (!name.empty()) {
        resp.setError(403, "Name already assigned");
    } else {
        const auto iterator = request.find("name");
        if (iterator == request.end()) {
            resp.setError(422, "Missing argument: name");
        } else {
            auto requestedName = iterator->get<string>();
            if (container->addUserIfNotKnown(this, requestedName)) {
                name = requestedName;
                resp.fillOkResultIfNotSet();
            } else {
                resp.setError(409, "User exists");
            }
        }
    }
    return resp;

}

string StreamerClient::getName() {
    return name;
}

StreamerClient::~StreamerClient() {
    if (!name.empty()) {
        container->removeClient(this, name);
    }
}

ssize_t StreamerClient::sendMessage(const string &message) {
    return write(socketDescriptor, message.c_str(), message.size());
}
