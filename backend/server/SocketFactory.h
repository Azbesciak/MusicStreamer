#ifndef MUSICSTREAMER_SOCKETFACTORY_H
#define MUSICSTREAMER_SOCKETFACTORY_H

#include <netinet/in.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>

class SocketFactory {

public:

    static int createTcpSocket(int port);
    static int createUdpSocket(int port);
};


#endif //MUSICSTREAMER_SOCKETFACTORY_H
