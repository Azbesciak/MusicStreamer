#ifndef MUSICSTREAMER_SOCKETFACTORY_H
#define MUSICSTREAMER_SOCKETFACTORY_H


#include "ServerRunner.h"

class SocketFactory {

public:

    static int createTcpSocket(int port);
};


#endif //MUSICSTREAMER_SOCKETFACTORY_H
