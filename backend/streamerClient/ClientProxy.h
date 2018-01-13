
#ifndef MUSICSTREAMER_CLIENTCOMMAND_H
#define MUSICSTREAMER_CLIENTCOMMAND_H


#include <upload/FileUpload.h>
#include "ClientResponse.h"
#include "logic/Container.h"

class FileUpload;

class ClientProxy {
    Container* container;
    StreamerClient* client;
    ClientResponse authenticate(const string &method, json request);

    void onUploadCompleted(FileUpload* fileUpload);
    void onUploadFailed(FileUpload* fileUpload);
    bool reserveRoomFileSlot();

public:
    ClientProxy(int clientFd, Container * container);
    ~ClientProxy();
    ClientResponse onNewMessage(char *message);

    ssize_t sendMessage(const string &message);

    bool isNotAuthorized() const;

    ClientResponse handleTrackUpload();
};


#endif //MUSICSTREAMER_CLIENTCOMMAND_H
