#include "TrackUpload.h"


void TrackUpload::onUploadCompleted(UploadedFile* uploadedFile) {
    FileUpload::onUploadCompleted(uploadedFile);

    track->setTrackFile(uploadedFile);
}


void TrackUpload::onUploadFailed() {
    FileUpload::onUploadFailed();

    room->cancelTrackReservation(track);
}
