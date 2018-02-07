#ifndef MUSICSTREAMER_FILEUPLOAD_H
#define MUSICSTREAMER_FILEUPLOAD_H

#include <iostream>
#include "UploadedFile.h"


class ClientProxy;

class FileUpload {

private:

    long fileSize;

public:

    FileUpload(long fileSize);

    long getFileSize();

    virtual void onUploadCompleted(UploadedFile* uploadedFile);
    virtual void onUploadFailed();

    ~FileUpload();
};


#endif //MUSICSTREAMER_UPLOADHANDLER_H
