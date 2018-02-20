
#ifndef MUSICSTREAMER_CLIENTCOMMAND_H
#define MUSICSTREAMER_CLIENTCOMMAND_H


#include <streamerClient/ClientResponse.h>
#include <logic/Container.h>
#include "Request.h"
#include "RequestProcessor.h"

class MusicTrack;

class ClientProxy: public RequestProcessor {
    Container* container;
    StreamerClient* client;
    ClientResponse authenticate(const string &method, Request * request);
    ClientResponse onNewRequest(Request *request, const string &method, ClientResponse *&response) override;
    MusicTrack* reserveRoomTrackSlot(const std::string& trackName);

public:
    ClientProxy(int clientFd, const string &addr, Container * container);
    ~ClientProxy();

    ssize_t respond(const string &message);

    bool isNotAuthorized() const;

    ClientResponse handleTrackUpload(const std::string& trackName, int fileSize);
    ClientResponse handleQueueTrack(const std::string& trackName);
    ClientResponse handleNextTrack();
    ClientResponse handleTracksRequest();
    ClientResponse handleQueueRequest();
    ClientResponse handleReorderTrack(int fromIndex, int toIndex);

    ClientResponse onNewRequest(Request *request) override;
};


#endif //MUSICSTREAMER_CLIENTCOMMAND_H
