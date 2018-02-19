#ifndef MUSICSTREAMER_UPLOADHANDLER_H
#define MUSICSTREAMER_UPLOADHANDLER_H


#include <logic/Room.h>
#include <set>
#include <netinet/in.h>
#include <map>
#include <server/TcpServer.h>
#include "TrackUpload.h"

#define TOKEN_SIZE 20


class UploadHandler : public TcpServer {
protected:
    void onNewConnection(int clientSocket, const string &remoteAddr) override;

private:
    class UploadRequestProcessor: public RequestProcessor {
    public:
        virtual ~UploadRequestProcessor();

    private:
        ClientResponse onNewRequest(Request *request, const string &method, ClientResponse *&response) override;
    public:
        int clientSocket;
        TrackUpload * upload;
        StreamerClient * client;

        explicit UploadRequestProcessor(int clientSocket);

        ClientResponse onNewRequest(Request *request) override;
        ssize_t respond(ClientResponse * response);
    };

    static const int MAX_SIMULTANEOUS_UPLOADS = 20;
    static const int MAX_FILE_SIZE = 100 * 1024 * 1024; // 100 MB
    static const int UPLOAD_TIMEOUT_MILLIS = 5000; // 5 s


    static const int BYTE_BUFFER_SIZE = 1024;

    const string FILE_UPLOAD_DIRECTORY;

    static UploadHandler* instance;

    recursive_mutex mut;
    recursive_mutex fileMut;
    int nextFileNo;

    map<string, TrackUpload*> uploads;
    set<string> usedTokens;


    UploadHandler(const string &host, int port, ServerManager * manager, const string &storageDirectory);

    string generateToken();
    TrackUpload* retrieveUploadByToken(const string &token);

    void downloadFile(int clientSocket);
    UploadedFile *acceptFileBytes(UploadRequestProcessor *clientSocket);


    UploadedFile *createNewUploadedFile(UploadRequestProcessor *processor);

    ~UploadHandler() override;

public:
    static void initialize(const string &host, int port, ServerManager * manager, const string &storageDirectory);
    static void destroy();
    static UploadHandler* getInstance();

    string prepareUpload(TrackUpload* fileUpload);

    void setSocketTimeout(int clientSocket) const;

    string resolveNewFilePath();
};


#endif //MUSICSTREAMER_UPLOADHANDLER_H
