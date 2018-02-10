#include "MusicStreamer.h"

using namespace std;


typedef void* (*THREADFUNCPTR)(void*);

MusicStreamer::MusicStreamer(Room* room) {

    this->room = room;
    this->currentTrack = nullptr;
    this->streamerThread = new pthread_t();

    pthread_create(streamerThread, nullptr, (THREADFUNCPTR) &MusicStreamer::runStreamerExecutor, this);
}


void* MusicStreamer::runStreamerExecutor() {

    while (true) {


    }
}


MusicStreamer::~MusicStreamer() {

    pthread_cancel(*streamerThread);
    delete streamerThread;
}
