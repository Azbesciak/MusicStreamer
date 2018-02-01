#include <unistd.h>
#include "FileUpload.h"

using namespace std;


FileUpload::~FileUpload() = default;


void FileUpload::onUploadCompleted(int fileDescriptor) {
    // No action by default
}


void FileUpload::onUploadFailed() {
    // No action by default
}
