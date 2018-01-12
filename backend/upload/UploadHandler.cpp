#include <sys/socket.h>
#include <server/SocketFactory.h>
#include <fcntl.h>
#include "UploadHandler.h"

using namespace std;


const char* UploadHandler::FILE_UPLOAD_DIRECTORY = "uploaded-files/";

UploadHandler* UploadHandler::instance = nullptr;
std::recursive_mutex UploadHandler::mut;
int UploadHandler::nextFileNo = 1;

UploadHandler* UploadHandler::getInstance() {

    if (instance == nullptr)
        instance = new UploadHandler();

    return instance;
}

UploadHandler::UploadHandler() {
    // Just for singleton purpose
}

bool UploadHandler::acceptUpload(ClientProxy *client) {

    FileUpload* fileUpload = nullptr;

    synchronized(mut) {

        int receiverSocket = createReceiverSocket();
        sockets.push_back(receiverSocket);

        int file = createFile();

        fileUpload = new FileUpload(client, file);
        uploads.push_back(fileUpload);
    }

    return fileUpload != nullptr;
}

int UploadHandler::createFile() {

    string fileName = resolveNewFilename();

    int fd = creat(fileName.c_str(), O_WRONLY);

    if (fd < 0) {

        perror("Cannot create new file.\n");
        return -1;
    }

    close(fd);

    return fd;
}

string UploadHandler::resolveNewFilename() {
    return to_string(nextFileNo++);
}

int UploadHandler::createReceiverSocket() {
    return SocketFactory::createTcpSocket(UPLOAD_PORT);
}

UploadHandler::~UploadHandler() {

    for (int socket : sockets) {

        close(socket);
    }
}