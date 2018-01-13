#include <sys/socket.h>
#include <server/SocketFactory.h>
#include <fcntl.h>
#include <utility/token.hpp>
#include "UploadHandler.h"

using namespace std;


const char* const UploadHandler::FILE_UPLOAD_DIRECTORY = "uploaded-files/";

recursive_mutex UploadHandler::singlMut;
UploadHandler* UploadHandler::instance = nullptr;

UploadHandler* UploadHandler::getInstance() {

    if (instance == nullptr) {

        synchronized(singlMut) {

            if (instance == nullptr)
                instance = new UploadHandler();
        }
    }

    return instance;
}

UploadHandler::UploadHandler() {

    this->nextFileNo = 1;

    receiverSocket = SocketFactory::createTcpSocket(UPLOAD_PORT);

    spawnHandlerThread();
}

void UploadHandler::spawnHandlerThread() {

    listenerThread = new pthread_t();
    pthread_create(listenerThread, nullptr, listenerLoop, nullptr);
}

void* UploadHandler::listenerLoop(void*) {
    getInstance()->runLooper();
}

void UploadHandler::runLooper() {

    while (true) {

        sockaddr_in clientAddress;
        socklen_t addressSize = sizeof(sockaddr_in);

        int clientSocket = accept(receiverSocket, (sockaddr*) &clientAddress, &addressSize);

        if (clientSocket < 0) {

            perror("Client socket accept error.\n");
            continue;
        }

        logUploadConnection(clientAddress);
    }
}

void UploadHandler::logUploadConnection(sockaddr_in clientAddress) {

    char address[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(clientAddress.sin_addr), address, INET_ADDRSTRLEN);
    printf("File upload from: %s\n", address);
}

string UploadHandler::generateToken() {

    return TokenGenerator::alfanumeric(TOKEN_SIZE);
}

string UploadHandler::acceptUpload(ClientProxy *client) {

    string token;

    synchronized(mut) {

        int file = createFile();

        if (file != -1) {

            token = generateToken();
            FileUpload* fileUpload = new FileUpload(client, token, file);

            uploads.push_back(fileUpload);
        }
    }

    return token;
}

int UploadHandler::createFile() {

    string fileName = resolveNewFilePath();

    int fd = creat(fileName.c_str(), O_WRONLY);

    if (fd < 0) {

        perror("Cannot create new file.\n");
        return -1;
    }

    close(fd);

    return fd;
}

string UploadHandler::resolveNewFilePath() {

    string filename = to_string(nextFileNo++);

    return string(FILE_UPLOAD_DIRECTORY) + filename;
}

UploadHandler::~UploadHandler() {

    for (FileUpload* upload : uploads)
        upload->cancel();

    close(receiverSocket);

    pthread_cancel(*listenerThread);
}