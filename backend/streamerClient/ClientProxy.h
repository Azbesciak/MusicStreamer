
#ifndef MUSICSTREAMER_CLIENTCOMMAND_H
#define MUSICSTREAMER_CLIENTCOMMAND_H


#include <upload/FileUpload.h>
#include <upload/UploadHandler.h>
#include <streamerClient/ClientResponse.h>
#include <logic/Container.h>
#include "Request.h"


class ClientProxy {
    Container* container;
    StreamerClient* client;
    ClientResponse authenticate(const string &method, Request * request);

    MusicTrack* reserveRoomFileSlot();

public:
    ClientProxy(int clientFd, Container * container);
    ~ClientProxy();
    ClientResponse onNewMessage(Request * request);

    ssize_t sendMessage(const string &message);

    bool isNotAuthorized() const;

    ClientResponse handleTrackUpload(int fileSize);
};


#endif //MUSICSTREAMER_CLIENTCOMMAND_H
