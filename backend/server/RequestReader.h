
#ifndef MUSICSTREAMER_REQUESTREADER_H
#define MUSICSTREAMER_REQUESTREADER_H


#include <streamerClient/Request.h>

#define BUFFER_SIZE 1000

class RequestReader {
    char *buffer;
    int socketFd;
public:
    explicit RequestReader(int socketFd);
    Request* readRequest();
};


#endif //MUSICSTREAMER_REQUESTREADER_H
