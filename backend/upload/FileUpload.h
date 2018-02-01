#ifndef MUSICSTREAMER_FILEUPLOAD_H
#define MUSICSTREAMER_FILEUPLOAD_H

#include <iostream>


class ClientProxy;

class FileUpload {

public:

    virtual void downloadFile() = 0;

    ~FileUpload();
};


#endif //MUSICSTREAMER_UPLOADHANDLER_H
