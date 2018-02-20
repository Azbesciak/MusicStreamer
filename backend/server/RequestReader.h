
#ifndef MUSICSTREAMER_REQUESTREADER_H
#define MUSICSTREAMER_REQUESTREADER_H


#include <streamerClient/Request.h>

#define BUFFER_SIZE 256

class RequestReader {
    char *buffer;
    int socketFd;
    ssize_t readFromSocket();

public:
    virtual ~RequestReader();

public:
    explicit RequestReader(int socketFd);
    std::vector<Request*> readRequest();
};


#endif //MUSICSTREAMER_REQUESTREADER_H
