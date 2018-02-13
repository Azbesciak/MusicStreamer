#include "UploadHandler.h"

#include <upload/exception/FileUploadException.h>
#include <utility/token.hpp>

#include <fcntl.h>
#include <server/RequestReader.h>
#include <fstream>

using namespace std;

UploadHandler* UploadHandler::instance = nullptr;

UploadHandler* UploadHandler::getInstance() {
    return instance;
}

UploadHandler::UploadHandler(const string &host, int port, ServerManager * manager, const string &storageDirectory):
        TcpServer(host, port, manager, "Upload server"), FILE_UPLOAD_DIRECTORY(storageDirectory),
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
    auto reader = new RequestReader(clientSocket);
    auto processor = new UploadRequestProcessor(clientSocket);

    auto requests = reader->readRequest();
    try {
        if (requests.size() > 1) {
            auto response = ClientResponse::error(429, "Hold on...");
            processor->respond(&response);
        } else {
            auto response = processor->onNewRequest(requests[0]);
            processor->respond(&response);

            if (processor->client != nullptr && processor->upload != nullptr) {
                UploadedFile* uploadedFile = acceptFileBytes(clientSocket, processor->upload->getFileSize());

                processor->upload->onUploadCompleted(uploadedFile);
                response = ClientResponse(200, "File uploaded successfully");
                processor->respond(&response);
            }
        }
    } catch(FileUploadException& ex) {

        if (processor->upload != nullptr)
            processor->upload->onUploadFailed();
        auto response = ClientResponse::error(ex.getStatusCode(), ex.what());
        processor->respond(&response);
    }
    for (auto req: requests) {
        delete req;
    }
    delete processor;
}

UploadedFile* UploadHandler::acceptFileBytes(int clientSocket, long fileSize) {
    UploadedFile* file = createNewUploadedFile();
    if (file == nullptr)
        throw FileUploadException(500, "Unexpected file upload error");

    long remainingSize = fileSize;
    auto buffer = new char[BYTE_BUFFER_SIZE + 1];
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

            } else if (bytes > remainingSize) {
                throw FileUploadException(400, "Received bigger file that it was specified");
            }
            ssize_t writeBytes = write(fileDescriptor, buffer, bytes);
            if (writeBytes < 0)
                throw FileUploadException(500, "Unexpected file write error");
            remainingSize -= bytes;
        }
    } catch(FileUploadException& ex) {
        close(fileDescriptor);
        remove(file->getFileName().c_str());
        delete buffer;
        throw ex;
    }
    delete buffer;
    close(fileDescriptor);
    return file;
}


FileUpload* UploadHandler::retrieveUploadByToken(const string &token) {

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

ClientResponse UploadHandler::UploadRequestProcessor::onNewRequest(Request *request, const string &method, ClientResponse *&response) {
    if (method == "UPLOAD_TOKEN") {
        auto clientName = request->getStr("name");
        auto handler = UploadHandler::getInstance();
        client = handler->manager->container->getClient(clientName);
        if (client == nullptr) {
            return ClientResponse::error(403, "Client not found");
        } else if (client -> getCurrentRoom() == nullptr){
            return ClientResponse::error(403, "Client has no room");
        }
        auto token = request->getStr("token");
        if (token.size() != TOKEN_SIZE) {
            response->setError(403, "Invalid Token");
        } else {
            upload = handler->retrieveUploadByToken(token);

            if (upload == nullptr)
                response->setError(404, "Upload with the token not found");
            else if (client->initializeUpload(clientSocket))
                return ClientResponse(100, "Now send the data");
            else {
                client = nullptr;
                response->setError(403, "Only one upload at the same time is allowed");
            }
        }
    } else {
        response->asUnknownResponse();
    }
    return *response;
}

ssize_t UploadHandler::UploadRequestProcessor::respond(ClientResponse * response) {
    if (response -> isError()) {
        cout<< RED_TEXT("ERROR on upload for client "
                                << clientSocket
                                << ": \n\t" << response->serialize()) << endl;
    } else {
        cout << "Sending "<< MAGENTA_TEXT("UPLOAD")
             << "response to client " << clientSocket
             <<": \n\t" << response->serialize() << endl;
    }
    return RequestProcessor::respond(clientSocket, response->serialize());
}

ClientResponse UploadHandler::UploadRequestProcessor::onNewRequest(Request *request) {
    return RequestProcessor::onNewRequest(request);
}

UploadHandler::UploadRequestProcessor::UploadRequestProcessor(int clientSocket):
        clientSocket(clientSocket),
        upload(nullptr),
        client(nullptr) {}

UploadHandler::UploadRequestProcessor::~UploadRequestProcessor() {
    delete upload;
    if (client != nullptr) {
        client->finishUpload(clientSocket);
    } else {
        close(clientSocket);
    }
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
        ofstream file(fileName);
        if (file.is_open()) {
            uploadedFile = new UploadedFile(fileName);
            file.close();
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

void UploadHandler::initialize(const string &host, int port, ServerManager * manager, const string &storageDirectory) {
    if (instance == nullptr) {
        instance = new UploadHandler(host, port, manager, storageDirectory);
    }
}

void UploadHandler::destroy() {
    delete instance;
    instance = nullptr;
}