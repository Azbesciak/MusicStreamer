#ifndef MUSICSTREAMER_UPLOADHANDLER_H
#define MUSICSTREAMER_UPLOADHANDLER_H


#include <upload/FileUpload.h>
#include <logic/Room.h>
#include <set>
#include <netinet/in.h>

class UploadHandler {

private:

    static const int MAX_SIMULTANEOUS_UPLOADS = 20;
    static const int MAX_FILE_SIZE = 10 * 1024 * 1024; // 10 MB
    static const int UPLOAD_TIMEOUT_MILLIS = 60000; // 60 s

    static const int UPLOAD_PORT = 21212;
    static const int TOKEN_SIZE = 20;

    static const char* const FILE_UPLOAD_DIRECTORY;


    static std::recursive_mutex singlMut;
    static UploadHandler* instance;

    std::recursive_mutex mut;
    pthread_t* listenerThread;

    int nextFileNo;

    std::vector<FileUpload*> uploads;
    std::set<std::string> usedTokens;

    int receiverSocket;

    UploadHandler();

    void spawnHandlerThread();
    static void* listenerLoop(void*);
    void runLooper();

    std::string generateToken();

    int createFile();
    std::string resolveNewFilePath();

    void logUploadConnection(sockaddr_in clientAddress);

public:

    static UploadHandler* getInstance();

    std::string acceptUpload(ClientProxy* client);

    ~UploadHandler();
};


#endif //MUSICSTREAMER_UPLOADHANDLER_H
