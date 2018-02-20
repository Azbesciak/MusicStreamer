
#include <upload/TrackUpload.h>
#include <streamerClient/ClientProxy.h>
#include <upload/exception/FileUploadException.h>
#include <upload/UploadHandler.h>


static const string JOIN_ROOM_ACTION = "JOIN";
static const string GET_ROOMS_ACTION = "ROOMS";
static const string AUTHENTICATE_ACTION = "INIT";
static const string LEAVE_ACTION = "LEAVE";
static const string UPLOAD_ACTION = "UPLOAD";
static const string STREAM_SUBSCRIBE = "STREAM_SUBSCRIBE";
static const string QUEUE_TRACK_ACTION = "QUEUE_TRACK";
static const string NEXT_TRACK_ACTION = "NEXT_TRACK";
static const string GET_TRACKS_ACTION = "TRACKS";
static const string GET_QUEUE_ACTION = "TRACKS_QUEUE";


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

    } else if (method == STREAM_SUBSCRIBE) {
        auto port = request->getInt("streamingPort");
        return container->setClientStreamingPort(client, port);

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

ClientResponse ClientProxy::handleTrackUpload(const string &trackName, int fileSize) {

    if (fileSize <= MusicTrack::WAV_HEADER_SIZE)
        return ClientResponse::error(403, "File too small. Only wav files are supported.");

    if (!client->hasRoomAssigned())
        return ClientResponse::error(403, "Client has no assigned room");

    MusicTrack *reservedTrack = reserveRoomTrackSlot(trackName);

    if (reservedTrack == nullptr)
        return ClientResponse::error(403, "Room tracks limit exceeded");

    TrackUpload *trackUpload = nullptr;
    string token = "";

    auto clientRoomName = client->getCurrentRoomName();

    try {
        trackUpload = new TrackUpload(reservedTrack, clientRoomName, fileSize);
        token = UploadHandler::getInstance()->prepareUpload(trackUpload);

    } catch (FileUploadException &e) {
        synchronized(*container->getRoomsMutex()) {
            auto clientRoom = container->getRoom(clientRoomName);
            clientRoom->cancelTrackReservation(reservedTrack);
        }
        delete trackUpload;

        return ClientResponse::error(e.getStatusCode(), e.what());
    }

    ClientResponse resp;
    resp.addToBody("uploadToken", token);
    return resp;
}


ClientResponse ClientProxy::handleQueueTrack(const std::string &trackName) {
    return Container::withRoom(client, [&](Room * room) {
        auto wasAdded = room->queueTrack(trackName);
        if (!wasAdded) {
            return ClientResponse::error(404, "Track not found");
        }
        return ClientResponse::ok();
    });
}


ClientResponse ClientProxy::handleNextTrack() {
    return Container::withRoom(client, [&](Room *room) {
        room->switchTrack();
        return ClientResponse::ok();
    });
}


ClientResponse ClientProxy::handleTracksRequest() {
    return Container::withRoom(client, [&](Room *room) {
        ClientResponse response = room->createBasicResponse();
        room->prepareTracksResponse(&response);
        return response;
    });
}


ClientResponse ClientProxy::handleQueueRequest() {
    return Container::withRoom(client, [&](Room *room) {
        ClientResponse response = room->createBasicResponse();
        room->prepareTrackQueueResponse(&response);
        return response;
    });
}


MusicTrack *ClientProxy::reserveRoomTrackSlot(const string &trackName) {
    MusicTrack *track = nullptr;
    Container::withRoom(client->getCurrentRoomName(), [&](Room *room) {
        track = room->reserveTrackSlot(trackName);
    });
    return track;
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

ClientProxy::ClientProxy(int socketDf, const string &addr, Container *container)
        : container(container), client(new StreamerClient(socketDf, addr)) {}

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
