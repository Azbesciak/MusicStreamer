#ifndef MUSICSTREAMER_TRACKUPLOAD_H
#define MUSICSTREAMER_TRACKUPLOAD_H

#include <logic/MusicTrack.h>
#include <logic/Room.h>


class TrackUpload {

private:

    MusicTrack *track;
    string roomName;
    long fileSize;

public:

    TrackUpload(MusicTrack *track, const string &roomName, int fileSize)
            : fileSize(fileSize), roomName(roomName), track(track) {
    }

    void onUploadCompleted(UploadedFile *uploadedFile);
    void onUploadFailed();
    string getFileName();
    long getFileSize();
};


#endif //MUSICSTREAMER_TRACKUPLOAD_H
