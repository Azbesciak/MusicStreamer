#include "MusicTrack.h"

#include <utility/synch.h>
#include <unistd.h>
#include <fcntl.h>
#include <cmath>

using namespace std;


MusicTrack::MusicTrack(const string &trackName) {

    this->trackName = trackName;
    this->trackFile = nullptr;
    this->openedTrackFile = -1;
    this->trackHeader = nullptr;
}


const string &MusicTrack::getTrackName() {
    return trackName;
}


void MusicTrack::setTrackFile(UploadedFile *trackFile) {

    synchronized(trackMut) {
        this->trackFile = trackFile;
    }
}


void MusicTrack::openTrack() {

    synchronized(trackMut) {

        if (trackFile != nullptr && !isOpened()) {

            openedTrackFile = open(trackFile->getFilePath().c_str(), O_RDONLY);
            headerProcessed = false;
            trackFinished = false;
        }
    }
}


bool MusicTrack::isSaved() {
    return trackFile != nullptr;
}


bool MusicTrack::isOpened() {
    return openedTrackFile > 0;
}


bool MusicTrack::isFinished() {
    return trackFinished;
}


void MusicTrack::closeTrack() {

    synchronized(trackMut) {

        if (isOpened()) {
            close(openedTrackFile);
            openedTrackFile = -1;
        }

        headerProcessed = false;
        trackFinished = false;
        delete[] trackHeader;
        trackHeader = nullptr;
    }
}


bool MusicTrack::readTrackHeader() {
    if (!headerProcessed) {
        synchronized(trackMut) {

            auto * headerBuffer = new char[WAV_HEADER_SIZE];
            ssize_t bytes = read(openedTrackFile, headerBuffer, WAV_HEADER_SIZE);

            if (bytes != WAV_HEADER_SIZE) {
                lseek(openedTrackFile, 0, SEEK_SET);
                cerr << "could not read header for track " << trackName << endl;
                delete headerBuffer;
                return false;
            } else {
                headerProcessed = true;
                trackHeader = headerBuffer;
                return true;
            }
        }
    }
    return true;
}


int MusicTrack::getSampleRate() {
    return trackHeader[24] + (trackHeader[25] << 8) + (trackHeader[26] << 16) + (trackHeader[27] << 24);
}


int MusicTrack::getBitsPerSample() {
    return trackHeader[34] + (trackHeader[35] << 8);
}


int MusicTrack::getChannelsNum() {
    return trackHeader[22] + (trackHeader[23] << 8);
}


int MusicTrack::getByteRate() {
    return trackHeader[28] + (trackHeader[29] << 8) + (trackHeader[30] << 16) + (trackHeader[31] << 24);
}


int MusicTrack::getSoundSize() {
    return trackHeader[40] + (trackHeader[41] << 8) + (trackHeader[42] << 16) + (trackHeader[43] << 24);
}


int MusicTrack::getChunkTimeGapMicrosec() {

    if (!readTrackHeader()) {
        return -1;
    }

    int soundSize = getSoundSize();
    int byteRate = getByteRate();

    double seconds = (double) soundSize / byteRate;
    double chunks = (double) soundSize / SOUND_CHUNK_SIZE;

    double result = seconds / chunks;

    return (int) (result * 1000000);
}


int MusicTrack::getTrackHeaderSize() {
    return WAV_HEADER_SIZE;
}


char *MusicTrack::getTrackHeader() {

    if (!isOpened() || !readTrackHeader())
        return nullptr;

    return trackHeader;
}


SoundChunk* MusicTrack::nextSoundChunk() {

    if (!isOpened() || !headerProcessed)
        return nullptr;

    char *chunkBuffer = new char[SOUND_CHUNK_SIZE + 1];
    ssize_t bytes = read(openedTrackFile, chunkBuffer, SOUND_CHUNK_SIZE);

    if (bytes == 0)
        trackFinished = true;

    return new SoundChunk(chunkBuffer, bytes);
}


MusicTrack::~MusicTrack() {
    if (isOpened())
        close(openedTrackFile);
    if (trackFile != nullptr) {
        remove(trackFile->getFilePath().c_str());
        delete trackFile;
    }
    delete[] trackHeader;
}
