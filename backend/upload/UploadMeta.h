#ifndef MUSICSTREAMER_UPLOADMETA_H
#define MUSICSTREAMER_UPLOADMETA_H


#include <sys/types.h>
#include <netdb.h>
#include <upload/UploadHandler.h>

class UploadHandler;

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
