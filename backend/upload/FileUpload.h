#ifndef MUSICSTREAMER_UPLOADHANDLER_H
#define MUSICSTREAMER_UPLOADHANDLER_H


#include <streamerClient/ClientProxy.h>

class FileUpload {

private:

    ClientProxy* clientProxy;
    int fileDescriptor;

public:

    FileUpload(ClientProxy* clientProxy, int fileDescriptor);

    void downloadFile();

    ~FileUpload();

};


#endif //MUSICSTREAMER_UPLOADHANDLER_H
