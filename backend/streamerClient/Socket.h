
#ifndef MUSICSTREAMER_SOCKET_H
#define MUSICSTREAMER_SOCKET_H


#include <unistd.h>

class Socket {
    int fd;
public:
    explicit Socket(int descriptor): fd(descriptor){};
    ~Socket() {
        close(fd);
    }
    int get() {
        return fd;
    }

};


#endif //MUSICSTREAMER_SOCKET_H
