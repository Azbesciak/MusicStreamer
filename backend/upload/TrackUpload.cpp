#include "TrackUpload.h"


void TrackUpload::onUploadCompleted(UploadedFile* uploadedFile) {
    FileUpload::onUploadCompleted(uploadedFile);
    track->setTrackFile(uploadedFile);
    Container::withRoom(roomName, [&] (Room * room) {
        room->onTrackListChanged();
    });

}


void TrackUpload::onUploadFailed() {
    FileUpload::onUploadFailed();
    Container::withRoom(roomName, [&] (Room * room) {
        room->cancelTrackReservation(track);
    });
}

string TrackUpload::getIdentify() {
    return roomName + "/" + track->getTrackName();
}

string TrackUpload::getFileName() {
    return track->getTrackName();
}
