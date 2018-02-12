#ifndef MUSICSTREAMER_UPLOADHANDLER_H
#define MUSICSTREAMER_UPLOADHANDLER_H


#include <upload/FileUpload.h>
#include <upload/UploadMeta.h>
#include <logic/Room.h>
#include <set>
#include <netinet/in.h>
#include <map>

class UploadMeta;

class UploadHandler {

private:

    static const int MAX_SIMULTANEOUS_UPLOADS = 20;
    static const int MAX_FILE_SIZE = 100 * 1024 * 1024; // 100 MB
    static const int UPLOAD_TIMEOUT_MILLIS = 50000; // 50 s

    static const int TOKEN_SIZE = 20;
    static const int BYTE_BUFFER_SIZE = 1024;

    static const char* const FILE_UPLOAD_DIRECTORY;


    static std::recursive_mutex singlMut;
    static UploadHandler* instance;

    std::recursive_mutex mut;
    std::recursive_mutex fileMut;
    pthread_t* listenerThread;

    int nextFileNo;

    std::map<std::string, FileUpload*> uploads;
    std::set<std::string> usedTokens;

    int receiverSocket;


    UploadHandler(const string &host, int port);

    void spawnHandlerThread();
    static void* listenerLoop(void*);
    void runLooper();

    std::string generateToken();
    FileUpload* retrieveUploadByToken(std::string token);

    void downloadFile(UploadMeta* uploadMeta);
    std::string acceptToken(int clientSocket);
    UploadedFile* acceptFileBytes(int clientSocket, long fileSize);


    UploadedFile* createNewUploadedFile();
    std::string resolveNewFilePath();

    void logUploadConnection(sockaddr_in clientAddress);
    void handleClientUpload(int clientSocket, sockaddr_in clientAddress);

    static void* handleFileDownload(void* data);
    ~UploadHandler();

public:
    static void initialize(const string &host, int port);
    static void destroy();
    static UploadHandler* getInstance();

    std::string prepareUpload(FileUpload* fileUpload);

};


#endif //MUSICSTREAMER_UPLOADHANDLER_H
