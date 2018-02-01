#include "TrackUpload.h"


void TrackUpload::downloadFile() {


}


void TrackUpload::onUploadCompleted(int fileDescriptor) {
    FileUpload::onUploadCompleted(fileDescriptor);

    // Todo handle file upload completed
}


void TrackUpload::onUploadFailed() {
    FileUpload::onUploadFailed();

    // Todo handle file upload failed
}
