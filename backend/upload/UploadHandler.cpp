#include "UploadHandler.h"

#include <upload/exception/FileUploadException.h>
#include <utility/token.hpp>

#include <fcntl.h>

using namespace std;

const char* const UploadHandler::FILE_UPLOAD_DIRECTORY = "uploaded-files/";

UploadHandler* UploadHandler::instance = nullptr;

UploadHandler* UploadHandler::getInstance() {
    return instance;
}

UploadHandler::UploadHandler(const string &host, int port, ServerManager * manager):
        TcpServer(host, port, manager, "Upload server"),
        nextFileNo(1) {}

void UploadHandler::onNewConnection(int clientSocket, const std::string &remoteAddr) {

    cout << MAGENTA_TEXT("File upload from: " << remoteAddr) << endl;
    setSocketTimeout(clientSocket);
    downloadFile(clientSocket);
}

void UploadHandler::setSocketTimeout(int clientSocket) const {
    timeval timeout {
            .tv_sec = UPLOAD_TIMEOUT_MILLIS / 1000,
            .tv_usec = (UPLOAD_TIMEOUT_MILLIS % 1000) * 1000
    };
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
}

void UploadHandler::downloadFile(int clientSocket) {

    StreamerClient client = StreamerClient(clientSocket);
    FileUpload* upload = nullptr;

    try {

        string token = acceptToken(clientSocket);

        client.sendMessage(ClientResponse(100, "Now send the data").serialize());

        upload = retrieveUploadByToken(token);

        if (upload == nullptr)
            throw FileUploadException(404, "Upload with the token not found");

        UploadedFile* uploadedFile = acceptFileBytes(clientSocket, upload->getFileSize());

        upload->onUploadCompleted(uploadedFile);
        client.sendMessage(ClientResponse(200, "File uploaded successfully").serialize());

        close(clientSocket);

    } catch(FileUploadException& ex) {

        if (upload != nullptr)
            upload->onUploadFailed();

        client.sendMessage(ClientResponse::error(ex.getStatusCode(), ex.what()).serialize());
    }

    delete upload;
}


UploadedFile* UploadHandler::acceptFileBytes(int clientSocket, long fileSize) {

    UploadedFile* file = createNewUploadedFile();

    if (file == nullptr)
        throw FileUploadException(500, "Unexpected file upload error");

    long remainingSize = fileSize;
    auto * buffer = new char[BYTE_BUFFER_SIZE + 1];

    int fileDescriptor = open(file->getFileName().c_str(), O_WRONLY);

    if (fileDescriptor < 0)
        throw FileUploadException(500, "Unexpected file upload error");

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

            ssize_t writeBytes = write(fileDescriptor, buffer, bytes);

            if (writeBytes < 0)
                throw FileUploadException(500, "Unexpected file write error");

            remainingSize -= bytes;
        }

    } catch(FileUploadException& ex) {

        close(fileDescriptor);
        remove(file->getFileName().c_str());

        throw ex;
    }

    close(fileDescriptor);

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

string UploadHandler::prepareUpload(FileUpload* fileUpload) {

    // Todo implement maximum number of uploads limit
    if (fileUpload->getFileSize() > MAX_FILE_SIZE)
        throw FileUploadException(400, "File size too large");

    string token = "";

    synchronized(mut) {

        token = generateToken();

        usedTokens.insert(token);
        uploads[token] = fileUpload;
    }

    return token;
}

string UploadHandler::generateToken() {

    string token = "";

    do {

        token = TokenGenerator::alfanumeric(TOKEN_SIZE);

    } while (usedTokens.find(token) != usedTokens.end());

    return token;
}

UploadedFile* UploadHandler::createNewUploadedFile() {

    UploadedFile* uploadedFile = nullptr;
    synchronized(fileMut) {

        string fileName = resolveNewFilePath();
        int fd = creat(fileName.c_str(), 0);

        if (fd >= 0) {

            uploadedFile = new UploadedFile(fileName);
            close(fd);
        }
    }

    return uploadedFile;
}

string UploadHandler::resolveNewFilePath() {

    string filename = to_string(nextFileNo++);

    return string(FILE_UPLOAD_DIRECTORY) + filename;
}

UploadHandler::~UploadHandler() {
    for (auto tokenUpload : uploads) {
        delete tokenUpload.second;
    }
}

void UploadHandler::initialize(const string &host, int port, ServerManager * manager) {
    if (instance == nullptr) {
        instance = new UploadHandler(host, port, manager);
    }
}

void UploadHandler::destroy() {
    delete instance;
    instance = nullptr;
}