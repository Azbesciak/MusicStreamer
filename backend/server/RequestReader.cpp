
#include "RequestReader.h"
#include "unistd.h"

RequestReader::RequestReader(int socketFd)
        :socketFd(socketFd), buffer(new char[BUFFER_SIZE]) {
}
ssize_t RequestReader::readFromSocket() {
    int newLinesLimit = 5;
    do {
        memset(buffer, 0, BUFFER_SIZE);
        auto r = read(socketFd, buffer, BUFFER_SIZE);
        if (buffer[0] == '\n') {
            continue;
        } else {
            return r;
        }
    } while (newLinesLimit-- > 0);
    return 0;
}

Request *RequestReader::readRequest() {
    std::string message;
    auto value = readFromSocket();

    if (buffer[0] != '<') {
        return new Request("invalid input format", false);
    } else if (value <= 0){
        return nullptr;
    }
    while (true) {
        value = readFromSocket();
        if (buffer[0] == '>') {
            return new Request(message);
        } else if (value <=0) {
            return nullptr;
        } else if (message.length() > MAX_READ_SIZE) {
            return new Request("Too long: " + message, false);
        } else {
            message += buffer[0];
        }
    }
}
