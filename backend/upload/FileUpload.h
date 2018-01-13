#ifndef MUSICSTREAMER_FILEUPLOAD_H
#define MUSICSTREAMER_FILEUPLOAD_H


#include <streamerClient/ClientProxy.h>

class FileUpload {

private:

    ClientProxy* clientProxy;
    int fileDescriptor;

    std::string token;

public:

    FileUpload(ClientProxy* clientProxy, std::string token, int fileDescriptor);

    void downloadFile();
    void cancel();

    ~FileUpload();

};


#endif //MUSICSTREAMER_UPLOADHANDLER_H
