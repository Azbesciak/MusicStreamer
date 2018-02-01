#ifndef MUSICSTREAMER_FILEUPLOAD_H
#define MUSICSTREAMER_FILEUPLOAD_H

#include <iostream>


class ClientProxy;

class FileUpload {

private:

    long fileSize;

public:

    virtual void downloadFile() = 0;

    virtual void onUploadCompleted(int fileDescriptor);
    virtual void onUploadFailed();

    ~FileUpload();
};


#endif //MUSICSTREAMER_UPLOADHANDLER_H
