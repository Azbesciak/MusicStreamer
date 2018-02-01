#include "UploadMeta.h"


UploadMeta::UploadMeta(UploadHandler* uploadHandlerInstance, pthread_t* handlerThread,
                       int clientSocket, sockaddr_in clientAddress) {

    this->uploadHandlerInstance = uploadHandlerInstance;
    this->handlerThread = handlerThread;
    this->clientSocket = clientSocket;
    this->clientAddress = clientAddress;
}

UploadHandler* UploadMeta::getUploadHandlerInstance() {
    return uploadHandlerInstance;
}

pthread_t* UploadMeta::getHandlerThread() {
    return handlerThread;
}

int UploadMeta::getClientSocket() {
    return clientSocket;
}

sockaddr_in UploadMeta::getClientAddress() {
    return clientAddress;std::string token;
}

UploadMeta::~UploadMeta() {
}


