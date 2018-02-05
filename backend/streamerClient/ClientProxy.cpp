
#include <upload/TrackUpload.h>
#include "ClientProxy.h"
#include "Request.h"


static const char *const JOIN_ROOM_ACTION = "JOIN";
static const char *const GET_ROOMS_ACTION = "ROOMS";
static const char *const AUTHENTICATE_ACTION = "INIT";
static const char *const LEAVE_ACTION = "LEAVE";
static const char *const UPLOAD_ACTION = "UPLOAD";

ClientResponse ClientProxy::onNewMessage(Request *req) {
    ClientResponse resp;

    if (!req->isCorrect()) {
        resp.setError(400, "Malformed input data");
    } else {
        string method = req->getMethod();

        if (method == AUTHENTICATE_ACTION) {

            return authenticate(method, req);

        } else if (isNotAuthorized()) {

            return ClientResponse::error(403, "Not authenticated");

        } else if (method == JOIN_ROOM_ACTION) {

            auto roomName = req->getStr("roomName");
            container->joinClientToRoom(client, roomName);

        } else if (method == GET_ROOMS_ACTION) {

            resp.addToBody("rooms", container->getRoomsList());

        } else if (method == UPLOAD_ACTION) {

            auto fileSize = req->getInt("trackFileSize");

            resp = handleTrackUpload(fileSize);
        }

        resp.fillOkResultIfNotSet();
    }

    return resp;
}

ClientResponse ClientProxy::handleTrackUpload(int fileSize) {

    if (client->getCurrentRoom())
        return ClientResponse::error(403, "Client has no assigned room");

    MusicTrack *reservedTrack = reserveRoomFileSlot();

    if (reservedTrack == nullptr)
        return ClientResponse::error(403, "Room tracks limit exceeded");

    auto *trackUpload = new TrackUpload(reservedTrack, this, fileSize);
    string token = UploadHandler::getInstance()->prepareUpload(trackUpload);

    if (token.empty()) {

        delete trackUpload;
        client->getCurrentRoom()->cancelTrackReservation(reservedTrack);

        return ClientResponse::error(409, "Cannot initiate file upload. Please try later");
    }

    ClientResponse resp;
    resp.addToBody("uploadToken", token);

    return resp;
}


MusicTrack *ClientProxy::reserveRoomFileSlot() {

    Room *room = client->getCurrentRoom();

    if (room == nullptr)
        return nullptr;

    return room->reserveTrackSlot();
}


bool ClientProxy::isNotAuthorized() const { return client->getName().empty(); }


ClientResponse ClientProxy::authenticate(const string &method, Request *request) {
    ClientResponse resp;
    if (!client->getName().empty()) {
        resp.setError(403, "Name already assigned");
    } else {
        if (!request->has("name")) {
            resp.setError(422, "Missing argument: name");
        } else {
            auto requestedName = request->getStr("name");
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


