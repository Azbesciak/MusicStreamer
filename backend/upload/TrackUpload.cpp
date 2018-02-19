#include "TrackUpload.h"


void TrackUpload::onUploadCompleted(UploadedFile* uploadedFile) {
    track->setTrackFile(uploadedFile);
    Container::withRoom(roomName, [&] (Room * room) {
        room->onTrackListChanged();
    });
}

long TrackUpload::getFileSize() {
    return fileSize;
}

void TrackUpload::onUploadFailed() {
    Container::withRoom(roomName, [&] (Room * room) {
        room->cancelTrackReservation(track);
    });
}

string TrackUpload::getFileName() {
    return track->getTrackName();
}
