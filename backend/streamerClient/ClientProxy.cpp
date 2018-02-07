
#include <upload/TrackUpload.h>
#include <streamerClient/ClientProxy.h>


static const char *const JOIN_ROOM_ACTION = "JOIN";
static const char *const GET_ROOMS_ACTION = "ROOMS";
static const char *const AUTHENTICATE_ACTION = "INIT";
static const char *const LEAVE_ACTION = "LEAVE";
static const char *const UPLOAD_ACTION = "UPLOAD";
static const char *const QUEUE_TRACK_ACTION = "QUEUE_TRACK";
static const char *const PAUSE_TRACK_ACTION = "PAUSE_TRACK";
static const char *const RESUME_TRACK_ACTION = "RESUME_TRACK";
static const char *const NEXT_TRACK_ACTION = "NEXT_TRACK";

static const int WAV_HEADER_SIZE = 44;

ClientResponse ClientProxy::onNewRequest(Request *request, const string &method, ClientResponse *&response) {

    if (method == AUTHENTICATE_ACTION) {

        return authenticate(method, request);

    } else if (isNotAuthorized()) {

        return ClientResponse::error(403, "Not authenticated");

    } else if (method == JOIN_ROOM_ACTION) {

        auto roomName = request->getStr("roomName");

        container->joinClientToRoom(client, roomName);

    } else if (method == LEAVE_ACTION) {
        container->removeClientFromRooms(client);

    } else if (method == GET_ROOMS_ACTION) {

        return container->createRoomsResponse();

    } else if (method == UPLOAD_ACTION) {

        string fileName = request->getStr("trackName");
        int fileSize = request->getInt("trackFileSize");

        return handleTrackUpload(fileSize);

    } else {

        response->asUnknownResponse();
    }

    return *response;
}

ClientResponse ClientProxy::handleTrackUpload(int fileSize) {

    if (fileSize <= WAV_HEADER_SIZE)
        return ClientResponse::error(403, "File is too small. Only wav files are supported.");

    if (client->getCurrentRoom())
        return ClientResponse::error(403, "Client has no assigned room");

    MusicTrack *reservedTrack = reserveRoomFileSlot();

    if (reservedTrack == nullptr)
        return ClientResponse::error(403, "Room tracks limit exceeded");

    TrackUpload* trackUpload = new TrackUpload(reservedTrack, client->getCurrentRoom(), fileSize);
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

    Room* room = client->getCurrentRoom();

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

ClientResponse ClientProxy::onNewRequest(Request *request) {
    return RequestProcessor::onNewRequest(request);
}

ssize_t ClientProxy::respond(const string &message) {
    return client->sendMessage(message);
}



