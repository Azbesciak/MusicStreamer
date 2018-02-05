#include "UploadHandler.h"

#include <server/SocketFactory.h>
#include <upload/exception/FileUploadException.h>
#include <utility/token.hpp>

#include <fcntl.h>
#include <utility/synch.h>
#include <streamerClient/ClientResponse.h>
#include <unistd.h>
#include <arpa/inet.h>

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
    this->receiverSocket = SocketFactory::createTcpSocket("127.0.0.1", UPLOAD_PORT); //TODO

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

    pthread_create(thread, nullptr, handleFileDownload, uploadMeta);
}


void* UploadHandler::handleFileDownload(void* metadata) {

    auto * uploadMeta = static_cast<UploadMeta*>(metadata);
    UploadHandler* handler = uploadMeta->getUploadHandlerInstance();

    // Set socket read timeout
    timeval timeout{};
    timeout.tv_sec = UPLOAD_TIMEOUT_MILLIS / 1000;
    timeout.tv_usec = (UPLOAD_TIMEOUT_MILLIS % 1000) * 1000;
    setsockopt(uploadMeta->getClientSocket(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    handler->downloadFile(uploadMeta);

    return nullptr;
}


void UploadHandler::downloadFile(UploadMeta* uploadMeta) {

    StreamerClient client = StreamerClient(uploadMeta->getClientSocket());
    FileUpload* upload = nullptr;

    try {

        string token = acceptToken(uploadMeta->getClientSocket());

        client.sendMessage(ClientResponse(100, "Now send the data").serialize());

        upload = retrieveUploadByToken(token);

        if (upload == nullptr)
            throw FileUploadException(404, "Upload with the token not found");

        int fileDescriptor = acceptFileBytes(uploadMeta->getClientSocket(), upload->getFileSize());

        upload->onUploadCompleted(fileDescriptor);
        client.sendMessage(ClientResponse(200, "File uploaded successfully").serialize());

        close(uploadMeta->getClientSocket());

    } catch(FileUploadException& ex) {

        if (upload != nullptr)
            upload->onUploadFailed();

        client.sendMessage(ClientResponse::error(ex.getStatusCode(), ex.what()).serialize());

    }

    delete upload;
}


int UploadHandler::acceptFileBytes(int clientSocket, int fileSize) {

    int file = createFile();

    if (file < 0)
        throw FileUploadException(500, "Unexpected file upload error");

    int remainingSize = fileSize;
    auto * buffer = new char[BYTE_BUFFER_SIZE + 1];

    try {

        while (remainingSize > 0) {

            ssize_t bytes = read(clientSocket, buffer, BYTE_BUFFER_SIZE);

            if (bytes < 0) {

                if (errno == EWOULDBLOCK)
                    throw FileUploadException(408, "File upload timeout");
                else
                    throw FileUploadException(500, "Unexpected file upload error");

            } else if (bytes == 0) {
                throw FileUploadException(400, "Received smaller file than it was specified");

            } else if (bytes > remainingSize)
                throw FileUploadException(400, "Received bigger file that it was specified");

            ssize_t writeBytes = write(file, buffer, bytes);

            if (writeBytes < 0)
                throw FileUploadException(500, "Unexpected file write error");

            remainingSize -= bytes;
        }

    } catch(FileUploadException& ex) {

        ftruncate(file, 0);
        close(file);
        throw ex;
    }

    close(file);
    return file;
}


FileUpload* UploadHandler::retrieveUploadByToken(string token) {

    FileUpload* upload = nullptr;

    synchronized(mut) {

        auto tokenUpload = uploads.find(token);

        if (tokenUpload != uploads.end()) {

            upload = tokenUpload->second;

            uploads.erase(tokenUpload);
            usedTokens.erase(token);
        }
    }

    return upload;
}


string UploadHandler::acceptToken(int clientSocket) {

    auto * tokenBuffer = new char[TOKEN_SIZE + 1];
    size_t remainingBytes = TOKEN_SIZE;

    string token;

    while (remainingBytes > 0) {

        long bytes;

        if ((bytes = read(clientSocket, tokenBuffer, remainingBytes)) > 0) {

            tokenBuffer[bytes] = '\0';
            token += tokenBuffer;
            remainingBytes -= bytes;

        } else if (bytes == 0) {
            throw FileUploadException(400, "Closed connection before uploading file content");

        } else if (errno == EWOULDBLOCK) {
            throw FileUploadException(408, "File upload timeout");

        } else {
            throw FileUploadException(500, "Unexpected file upload error");
        }
    }

    return tokenBuffer;
}


void UploadHandler::logUploadConnection(sockaddr_in clientAddress) {

    char address[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(clientAddress.sin_addr), address, INET_ADDRSTRLEN);
    printf("File upload from: %s\n", address);
}


string UploadHandler::prepareUpload(FileUpload* fileUpload) {

    string token = "";

    synchronized(mut) {

        token = generateToken();
        uploads[token] = fileUpload;
    }

    return token;
}


string UploadHandler::generateToken() {

    string token = "";

    do {

        token = TokenGenerator::alfanumeric(TOKEN_SIZE);

    } while(usedTokens.find(token) != usedTokens.end());

    usedTokens.insert(token);

    return token;
}


int UploadHandler::createFile() {

    int fd = -1;

    synchronized(fileMut) {

        string fileName = resolveNewFilePath();
        fd = creat(fileName.c_str(), O_WRONLY);

        if (fd < 0)
            perror("Cannot create new file.\n");
    }

    return fd;
}


string UploadHandler::resolveNewFilePath() {

    string filename = to_string(nextFileNo++);

    return string(FILE_UPLOAD_DIRECTORY) + filename;
}


UploadHandler::~UploadHandler() {

    for (auto tokenUpload : uploads) {

        FileUpload* upload = tokenUpload.second;
        delete upload;
    }

    pthread_cancel(*listenerThread);
    close(receiverSocket);
}