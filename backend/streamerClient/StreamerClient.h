
#ifndef MUSICSTREAMER_STREAMERCLIENT_H
#define MUSICSTREAMER_STREAMERCLIENT_H

#include "StreamerClient.h"

class StreamerClient {
private:
    int socketDescriptor;

public:
    explicit StreamerClient(int socketDescriptor);



};


#endif //MUSICSTREAMER_STREAMERCLIENT_H
