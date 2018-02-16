#include "TrackUpload.h"


void TrackUpload::onUploadCompleted(UploadedFile* uploadedFile) {
    FileUpload::onUploadCompleted(uploadedFile);
    track->setTrackFile(uploadedFile);
}


void TrackUpload::onUploadFailed() {
    FileUpload::onUploadFailed();
    auto cont = Container::getInstance();
    synchronized(*cont ->getRoomsMutex()) {
        auto room = cont ->getRoom(roomName);
        if (room != nullptr) {
            room->cancelTrackReservation(track);
        }
    }
}
