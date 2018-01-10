
#ifndef MUSICSTREAMER_CLIENTCOMMAND_H
#define MUSICSTREAMER_CLIENTCOMMAND_H


#include "ClientResponse.h"
#include "../server/Container.h"

static const char *const METHOD_KEY = "method";
static const char *const JOIN_ROOM_ACTION = "JOIN";
static const char *const GET_ROOMS_ACTION = "ROOMS";
static const char *const AUTHENTICATE_ACTION = "INIT";
static const char *const LEAVE_ACTION = "LEAVE";

class ClientProxy {
    Container* container;
    StreamerClient* client;
    ClientResponse authenticate(const string &method, json request);
public:
    ClientProxy(int clientFd, Container * container);
    ~ClientProxy();
    ClientResponse onNewMessage(char *message);

    ssize_t sendMessage(const string &message);
};


#endif //MUSICSTREAMER_CLIENTCOMMAND_H
