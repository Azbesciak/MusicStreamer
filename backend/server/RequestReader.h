
#ifndef MUSICSTREAMER_REQUESTREADER_H
#define MUSICSTREAMER_REQUESTREADER_H


#include <streamerClient/Request.h>

#define BUFFER_SIZE 1
#define MAX_READ_SIZE 4096

class RequestReader {
    char *buffer;
    int socketFd;
    ssize_t readFromSocket();

public:
    explicit RequestReader(int socketFd);
    Request* readRequest();

};


#endif //MUSICSTREAMER_REQUESTREADER_H
