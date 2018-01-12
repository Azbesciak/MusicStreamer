#ifndef MUSICSTREAMER_UPLOADHANDLERFACTORY_H
#define MUSICSTREAMER_UPLOADHANDLERFACTORY_H


#include <upload/FileUpload.h>
#include <streamerClient/Room.h>

class UploadHandler {

private:

    static const int MAX_SIMULTANEOUS_UPLOADS = 20;
    static const int UPLOAD_PORT = 21212;

    static UploadHandler* instance;

    std::vector<FileUpload*> uploads;


    UploadHandler();

    int createReceiverSocket();

public:

    static UploadHandler* getInstance();

    FileUpload* acceptUpload(StreamerClient* client);

    ~UploadHandler();
};


#endif //MUSICSTREAMER_UPLOADHANDLERFACTORY_H
