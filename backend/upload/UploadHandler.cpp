#include <sys/socket.h>
#include <server/SocketFactory.h>
#include <fcntl.h>
#include <utility/token.hpp>
#include "UploadHandler.h"
#include "UploadMeta.h"

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

        handleClientUpload(clientSocket, clientAddress);
    }
}


void UploadHandler::handleClientUpload(int clientSocket, sockaddr_in clientAddress) {

    logUploadConnection(clientAddress);

    pthread_t* thread = new pthread_t();
    UploadMeta* uploadMeta = new UploadMeta(this, thread, clientSocket, clientAddress);

    pthread_create(thread, nullptr, downloadFile, uploadMeta);
}


void* UploadHandler::downloadFile(void* metadata) {

    UploadMeta* uploadMeta = static_cast<UploadMeta*>(metadata);
    UploadHandler* handler = uploadMeta->getUploadHandlerInstance();

    string token = acceptToken(uploadMeta->getClientSocket());

    return nullptr;
}


string UploadHandler::acceptToken(int clientSocket) {

    char* tokenBuffer = new char[TOKEN_SIZE + 1];

    if (read(clientSocket, tokenBuffer, TOKEN_SIZE) != 0)
        return "";

    return tokenBuffer;
}


void UploadHandler::logUploadConnection(sockaddr_in clientAddress) {

    char address[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(clientAddress.sin_addr), address, INET_ADDRSTRLEN);
    printf("File upload from: %s\n", address);
}

string UploadHandler::generateToken() {

    return TokenGenerator::alfanumeric(TOKEN_SIZE);
}

string UploadHandler::prepareUpload(FileUpload* fileUpload) {

    string token;

    synchronized(mut) {

        token = generateToken();
        uploads.push_back(fileUpload);
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
        delete upload;

    close(receiverSocket);

    pthread_cancel(*listenerThread);
}