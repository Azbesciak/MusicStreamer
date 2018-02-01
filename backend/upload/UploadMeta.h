#ifndef MUSICSTREAMER_UPLOADMETA_H
#define MUSICSTREAMER_UPLOADMETA_H


#include <sys/types.h>
#include "UploadHandler.h"

class UploadMeta {

private:

    UploadHandler* uploadHandlerInstance;
    pthread_t* handlerThread;

    int clientSocket;
    sockaddr_in clientAddress;

public:

    UploadMeta(UploadHandler* uploadHandlerInstance, pthread_t* handlerThread,
               int clientSocket, sockaddr_in clientAddress);


    UploadHandler* getUploadHandlerInstance();
    pthread_t* getHandlerThread();
    int getClientSocket();
    sockaddr_in getClientAddress();

    ~UploadMeta();
};


#endif //MUSICSTREAMER_UPLOADMETA_H
