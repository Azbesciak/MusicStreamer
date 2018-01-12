#include "FileUpload.h"


FileUpload::FileUpload(ClientProxy *clientProxy, int fileDescriptor) {

    this->clientProxy = clientProxy;
    this->fileDescriptor = fileDescriptor;
}

void FileUpload::downloadFile() {}

FileUpload::~FileUpload() {}
