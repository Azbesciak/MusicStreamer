#ifndef MUSICSTREAMER_SOCKETFACTORY_H
#define MUSICSTREAMER_SOCKETFACTORY_H

#include <netinet/in.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <string>


#define QUEUE_SIZE 128

class SocketFactory {
    static int createSocket(const std::string &host, int port, int type, bool exitOnFail);

    static const int INCORRECT_SOCKET = 1;

public:
    static int createTcpSocket(const std::string &host, int port);

    static int createUdpSocket(const std::string &host, int port);

    static sockaddr_in getSockAddr(const std::string &host, int port);
};


#endif //MUSICSTREAMER_SOCKETFACTORY_H
