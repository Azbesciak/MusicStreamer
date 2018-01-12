#include <sys/socket.h>
#include <netinet/in.h>
#include "UploadHandler.h"


UploadHandler* UploadHandler::instance = nullptr;

UploadHandler* UploadHandler::getInstance() {

    if (instance == nullptr)
        instance = new UploadHandler();

    return instance;
}

UploadHandler::UploadHandler() {
}

FileUpload* UploadHandler::acceptUpload(StreamerClient *client) {}

int UploadHandler::createReceiverSocket() {

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    return sock;
}

UploadHandler::~UploadHandler() {}