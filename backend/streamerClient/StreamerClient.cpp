
#include "../utility/json.hpp"
#include "StreamerClient.h"

using json = nlohmann::json;
using namespace std;

StreamerClient::StreamerClient(int socketDescriptor, Container* container):
        socketDescriptor(socketDescriptor),
        container(container) {}

ClientResponse StreamerClient::onNewMessage(char *message) {
    ClientResponse resp;
    try {
        auto request = json::parse(message);
        auto method = request.at(METHOD_KEY).get<string>();
        auto res = authenticate(method, request);
        switch (res) {
            case NOT_AUTHENTICATED: {
                resp.setError(403, "Not authenticated");
                return resp;
            }
            case NAME_EXISTS: {
                resp.setError(409, "User exists");
                return resp;
            }
            case NAME_ASSIGNED: {
                resp.setError(403, "Name already assigned");
                return resp;
            }
            case AUTHENTICATED: {
                if (method == JOIN_ROOM_ACTION) {
                    auto roomName = request.at("roomName").get<string>();
                    container->joinClientToRoom(this, roomName);
                } else if (method == "ROOMS") {
                    resp.addToBody("rooms", container->getRoomsList());

                }
                resp.fillOkResultIfNotSet();
            };
        }

    } catch (json::exception &e) {
        cout << e.what() << endl;
        resp.setError(500, "Malformed input data");
    }
    return resp;
}

AuthRes StreamerClient::authenticate(string method, json request) {
    if (method == "INIT") {
        if (!name.empty()) {
            return NAME_ASSIGNED;
        }
        auto requestedName = request.at("name").get<string>();
        if (container->addUserIfNotKnown(this, requestedName)) {
            name = requestedName;
        } else {
            return NAME_EXISTS;
        }
    }
    return name.empty() ? NOT_AUTHENTICATED : AUTHENTICATED;
}

string StreamerClient::getName() {
    return name;
}

StreamerClient::~StreamerClient() {
    if (!name.empty()) {
        container->removeClient(name);
    }
}
