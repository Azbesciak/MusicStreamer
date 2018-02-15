#include <arpa/inet.h>
#include "SocketFactory.h"


int SocketFactory::createSocket(const std::string &host, int port, int type, bool exitOnFail) {
    int socketNum = socket(AF_INET, type, 0);
    if (socketNum < 0) {
        printf("Socket error\n");
        exit(-1);
    }
    sockaddr_in sockAddr = getSockAddr(host, port);

    int time = 1;
    setsockopt(socketNum, SOL_SOCKET, SO_REUSEADDR, (char *) &time, sizeof(time));
    if (bind(socketNum, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) < 0) {
        if (exitOnFail) {
            perror("Binding error");
            exit(-1);
        } else {
            return INCORRECT_SOCKET;
        }
    }

    return socketNum;
}

sockaddr_in SocketFactory::getSockAddr(const std::string &host, int port) {
    return sockaddr_in {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = {inet_addr(host.c_str())}
    };
}

int SocketFactory::createTcpSocket(const std::string &host, int port) {
    int socket = createSocket(host, port, SOCK_STREAM, true);
    if (listen(socket, QUEUE_SIZE) < 0) {
        perror("Listen error");
        exit(-1);
    }
    return socket;
}

int SocketFactory::createUdpSocket(const std::string &host, int port) {
    return createSocket(host, port, SOCK_DGRAM, false);
}

