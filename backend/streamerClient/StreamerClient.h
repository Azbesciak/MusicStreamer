#include <string>

#ifndef MUSICSTREAMER_STREAMERCLIENT_H
#define MUSICSTREAMER_STREAMERCLIENT_H

static const char *const METHOD_KEY = "method";

static const char *const JOIN_ROOM_ACTION = "POST";

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

};


#endif //MUSICSTREAMER_STREAMERCLIENT_H
