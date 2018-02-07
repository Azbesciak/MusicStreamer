#include <unistd.h>
#include "FileUpload.h"

using namespace std;


FileUpload::FileUpload(long fileSize) {

    this->fileSize = fileSize;
}


long FileUpload::getFileSize() {
    return fileSize;
}


void FileUpload::onUploadCompleted(UploadedFile* uploadedFile) {
    // No action by default
}


void FileUpload::onUploadFailed() {
    // No action by default
}


FileUpload::~FileUpload() = default;
