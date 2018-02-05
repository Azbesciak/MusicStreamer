#include "SocketFactory.h"


int SocketFactory::createSocket(int port, int type) {
    struct sockaddr_in sockAddr{};

    int socketNum = socket(AF_INET, type, 0);
    if (socketNum < 0) {
        printf("Socket error\n");
        exit(-1);
    }

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(static_cast<uint16_t>(port));
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int time = 1;
    setsockopt(socketNum, SOL_SOCKET, SO_REUSEADDR, (char *) &time, sizeof(time));
    if (bind(socketNum, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) < 0) {
        perror("Binding error");
        exit(-1);
    }

    return socketNum;
}

int SocketFactory::createTcpSocket(int port) {
    int socket = createSocket(port, SOCK_STREAM);
    if (listen(socket, QUEUE_SIZE) < 0) {
        perror("Listen error");
        exit(-1);
    }
    return socket;
}

int SocketFactory::createUdpSocket(int port) {
    return createSocket(port, SOCK_DGRAM);
}

