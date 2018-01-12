#include "SocketFactory.h"


int SocketFactory::createTcpSocket(int port) {

    struct sockaddr_in sockAddr{};

    int socketNum = socket(AF_INET, SOCK_STREAM, 0);
    if (socketNum < 0) {
        printf("Socket error\n");
        exit(-1);
    }
    //socket tylko uzywamy do czasu accept

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(static_cast<uint16_t>(port));
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bindowanie do socketu
    int time = 1;
    setsockopt(socketNum, SOL_SOCKET, SO_REUSEADDR, (char *) &time, sizeof(time));
    if (bind(socketNum, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) < 0) {
        perror("Binding error");
        exit(-1);
    }

    return socketNum;
}