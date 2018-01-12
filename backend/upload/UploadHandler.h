#ifndef MUSICSTREAMER_UPLOADHANDLERFACTORY_H
#define MUSICSTREAMER_UPLOADHANDLERFACTORY_H


#include <upload/FileUpload.h>
#include <logic/Room.h>

class UploadHandler {

private:

    static const int MAX_SIMULTANEOUS_UPLOADS = 20;
    static const int MAX_FILE_SIZE = 10 * 1024 * 1024; // 10 MB
    static const int UPLOAD_TIMEOUT_MILLIS = 60000; // 60 s

    static const int UPLOAD_PORT = 21212;

    static const char* FILE_UPLOAD_DIRECTORY;

    static UploadHandler* instance;
    static std::recursive_mutex mut;

    static int nextFileNo;

    std::vector<FileUpload*> uploads;
    std::vector<int> sockets;

    UploadHandler();

    int createReceiverSocket();
    int createFile();

    std::string resolveNewFilename();

public:

    static UploadHandler* getInstance();

    bool acceptUpload(ClientProxy* client);

    ~UploadHandler();
};


#endif //MUSICSTREAMER_UPLOADHANDLERFACTORY_H
