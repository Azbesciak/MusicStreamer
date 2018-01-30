
#include "ClientProxy.h"


static const char *const METHOD_KEY = "method";
static const char *const JOIN_ROOM_ACTION = "JOIN";
static const char *const GET_ROOMS_ACTION = "ROOMS";
static const char *const AUTHENTICATE_ACTION = "INIT";
static const char *const LEAVE_ACTION = "LEAVE";
static const char* const UPLOAD_ACTION = "UPLOAD";

ClientResponse ClientProxy::onNewMessage(char *message) {

    ClientResponse resp;

    try {

        auto request = json::parse(message);
        auto method = request.at(METHOD_KEY).get<string>();

        if (method == AUTHENTICATE_ACTION) {

            return authenticate(method, request);

        } else if (isNotAuthorized()) {

            return ClientResponse::error(403, "Not authenticated");

        } else if (method == JOIN_ROOM_ACTION) {

            auto roomName = request.at("roomName").get<string>();
            container->joinClientToRoom(client, roomName);

        } else if (method == GET_ROOMS_ACTION) {

            resp.addToBody("rooms", container->getRoomsList());

        } else if (method == UPLOAD_ACTION) {

            resp = handleTrackUpload();
        }

        resp.fillOkResultIfNotSet();

    } catch (json::exception &e) {

        cout << e.what() << endl;
        resp.setError(500, "Malformed input data");
    }

    return resp;
}

ClientResponse ClientProxy::handleTrackUpload() {

    bool reservation = reserveRoomFileSlot();

    if (!reservation)
        return ClientResponse::error(403, "Room tracks limit exceeded");

    string token = UploadHandler::getInstance()->acceptUpload(this);

    if (token.empty())
        return ClientResponse::error(409, "Cannot initiate file upload. Please try later");

    ClientResponse resp;

    resp.addToBody("uploadToken", token);

    return resp;
}

bool ClientProxy::reserveRoomFileSlot() {

    Room* room = client->getCurrentRoom();
    MusicTrack* trackSlot = room->reserveTrackSlot();
}

void ClientProxy::onUploadCompleted(FileUpload* fileUpload) {
}

void ClientProxy::onUploadFailed(FileUpload* fileUpload) {
}

bool ClientProxy::isNotAuthorized() const { return client->getName().empty(); }

ClientResponse ClientProxy::authenticate(const string &method, json request) {
    ClientResponse resp;
    if (!client->getName().empty()) {
        resp.setError(403, "Name already assigned");
    } else {
        const auto iterator = request.find("name");
        if (iterator == request.end()) {
            resp.setError(422, "Missing argument: name");
        } else {
            auto requestedName = iterator->get<string>();
            if (container->addUserIfNotKnown(client, requestedName)) {
                client->setName(requestedName);
                resp.fillOkResultIfNotSet();
            } else {
                resp.setError(409, "User exists");
            }
        }
    }
    return resp;
}

ClientProxy::ClientProxy(int socketDf, Container *container)
        : container(container), client(new StreamerClient(socketDf)) {}

ClientProxy::~ClientProxy() {
    if (client != nullptr && !client->getName().empty()) {
        container->removeClient(client);
    }
    delete client;
}

ssize_t ClientProxy::sendMessage(const string &message) {
    return client->sendMessage(message);
}


