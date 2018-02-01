#ifndef MUSICSTREAMER_TRACKUPLOAD_H
#define MUSICSTREAMER_TRACKUPLOAD_H

#include <logic/MusicTrack.h>
#include "FileUpload.h"


class TrackUpload : public FileUpload {

private:

    MusicTrack* track;
    ClientProxy* clientProxy;

public:

    TrackUpload(MusicTrack* track, ClientProxy* clientProxy, int fileSize)
        : FileUpload(fileSize) {

        this->track = track;
        this->clientProxy = clientProxy;
    }

    void downloadFile() override;

    void onUploadCompleted(int fileDescriptor) override;
    void onUploadFailed() override;
};


#endif //MUSICSTREAMER_TRACKUPLOAD_H
