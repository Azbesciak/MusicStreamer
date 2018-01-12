
#ifndef MUSICSTREAMER_CLIENTCOMMAND_H
#define MUSICSTREAMER_CLIENTCOMMAND_H


#include "ClientResponse.h"
#include "../server/Container.h"


class ClientProxy {
    Container* container;
    StreamerClient* client;
    ClientResponse authenticate(const string &method, json request);
public:
    ClientProxy(int clientFd, Container * container);
    ~ClientProxy();
    ClientResponse onNewMessage(char *message);

    ssize_t sendMessage(const string &message);

    bool isNotAuthorized() const;

    static void onUploadCompleted(ClientProxy* clientProxy, int fileDescriptor);
};


#endif //MUSICSTREAMER_CLIENTCOMMAND_H
