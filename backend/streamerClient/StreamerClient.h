#include <string>

#ifndef MUSICSTREAMER_STREAMERCLIENT_H
#define MUSICSTREAMER_STREAMERCLIENT_H

static const char *const METHOD_KEY = "method";
static const char *const JOIN_ROOM_ACTION = "JOIN";
static const char *const GET_ROOMS_ACTION = "ROOMS";
static const char *const AUTHENTICATE_ACTION = "INIT";
static const char *const LEAVE_ACTION = "LEAVE";


#include "StreamerClient.h"
#include "../server/Container.h"
#include "ClientResponse.h"

class StreamerClient {
private:
    int socketDescriptor;
    Container* container;
    string name;
    ClientResponse authenticate(const string &method, json request);

public:
    explicit StreamerClient(int socketDescriptor, Container* container);
    ~StreamerClient();
    ClientResponse onNewMessage(char *message);
    string getName();
    ssize_t sendMessage(const string &message);

};


#endif //MUSICSTREAMER_STREAMERCLIENT_H
