
#ifndef MUSICSTREAMER_STREAMERCLIENT_H
#define MUSICSTREAMER_STREAMERCLIENT_H

#include "StreamerClient.h"
#include "../server/Container.h"

class StreamerClient {
private:
    int socketDescriptor;
    Container* container;

public:
    explicit StreamerClient(int socketDescriptor, Container* container);
    void onNewMessage(char *message);
};


#endif //MUSICSTREAMER_STREAMERCLIENT_H
