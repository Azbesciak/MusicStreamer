#include "FileUpload.h"

using namespace std;


FileUpload::FileUpload(ClientProxy *clientProxy, string token, int fileDescriptor) {

    this->clientProxy = clientProxy;
    this->fileDescriptor = fileDescriptor;
}

void FileUpload::downloadFile() {}

void FileUpload::cancel() {}

FileUpload::~FileUpload() = default;
