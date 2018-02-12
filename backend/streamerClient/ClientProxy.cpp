
#include <upload/TrackUpload.h>
#include <streamerClient/ClientProxy.h>
#include <upload/exception/FileUploadException.h>


static const char *const JOIN_ROOM_ACTION = "JOIN";
static const char *const GET_ROOMS_ACTION = "ROOMS";
static const char *const AUTHENTICATE_ACTION = "INIT";
static const char *const LEAVE_ACTION = "LEAVE";
static const char *const UPLOAD_ACTION = "UPLOAD";
static const char *const QUEUE_TRACK_ACTION = "QUEUE_TRACK";
static const char *const NEXT_TRACK_ACTION = "NEXT_TRACK";
static const char *const GET_TRACKS_ACTION = "TRACKS";
static const char *const GET_QUEUE_ACTION = "TRACKS_QUEUE";


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

        string trackName = request->getStr("trackName");
        int fileSize = request->getInt("trackFileSize");

        return handleTrackUpload(trackName, fileSize);

    } else if (method == QUEUE_TRACK_ACTION) {

        string trackName = request->getStr("trackName");

        return handleQueueTrack(trackName);

    } else if (method == NEXT_TRACK_ACTION) {

        return handleNextTrack();

    } else if (method == GET_TRACKS_ACTION) {

        return handleTracksRequest();

    } else if (method == GET_QUEUE_ACTION) {

        return handleQueueRequest();

    } else {

        response->asUnknownResponse();
    }

    return *response;
}

ClientResponse ClientProxy::handleTrackUpload(const string& trackName, int fileSize) {

    if (fileSize <= MusicTrack::WAV_HEADER_SIZE)
        return ClientResponse::error(403, "File too small. Only wav files are supported.");

    if (client->getCurrentRoom() == nullptr)
        return ClientResponse::error(403, "Client has no assigned room");

    MusicTrack *reservedTrack = reserveRoomTrackSlot(trackName);

    if (reservedTrack == nullptr)
        return ClientResponse::error(403, "Room tracks limit exceeded");

    TrackUpload* trackUpload = nullptr;
    string token = "";

    try {

        trackUpload = new TrackUpload(reservedTrack, client->getCurrentRoom(), fileSize);
        token = UploadHandler::getInstance()->prepareUpload(trackUpload);

    } catch(FileUploadException& e) {

        client->getCurrentRoom()->cancelTrackReservation(reservedTrack);
        delete trackUpload;

        return ClientResponse::error(e.getStatusCode(), e.what());
    }

    ClientResponse resp;
    resp.addToBody("uploadToken", token);

    return resp;
}


ClientResponse ClientProxy::handleQueueTrack(const std::string& trackName) {

    Room* room = client->getCurrentRoom();

    if (room == nullptr)
        return ClientResponse::error(403, "Client has no assigned room");

    TracksQueue* tracksQueue = room->getTracksQueue();
    MusicTrack* track = room->findTrackByName(trackName);

    if (track == nullptr)
        return ClientResponse::error(404, "Track not found");

    tracksQueue->appendTrack(track);

    return ClientResponse::ok();
}


ClientResponse ClientProxy::handleNextTrack() {

    // Todo implement
    return ClientResponse::ok();
}


ClientResponse ClientProxy::handleTracksRequest() {

    // Todo implement
    return ClientResponse::ok();
}


ClientResponse ClientProxy::handleQueueRequest() {

    // Todo implement
    return ClientResponse::ok();
}


MusicTrack *ClientProxy::reserveRoomTrackSlot(const string& trackName) {

    Room* room = client->getCurrentRoom();

    if (room == nullptr)
        return nullptr;

    return room->reserveTrackSlot(trackName);
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



