
#include "RequestReader.h"
#include "unistd.h"

RequestReader::RequestReader(int socketFd)
        :socketFd(socketFd), buffer(new char[BUFFER_SIZE]) {

}

Request *RequestReader::readRequest() {
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t value = read(socketFd, buffer, BUFFER_SIZE);
    if (value > 0) {
        return new Request(buffer);
    }
    return nullptr;
}
