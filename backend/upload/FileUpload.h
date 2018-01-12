#ifndef MUSICSTREAMER_UPLOADHANDLER_H
#define MUSICSTREAMER_UPLOADHANDLER_H


#include <streamerClient/ClientProxy.h>

class FileUpload {

private:

    ClientProxy* clientProxy;
    int fileDescriptor;

public:

    FileUpload(ClientProxy* clientProxy);

    void downloadFile(void (*onComplete)(ClientProxy* clientProxy, int fileDescriptor));

    ~FileUpload();

};


#endif //MUSICSTREAMER_UPLOADHANDLER_H
