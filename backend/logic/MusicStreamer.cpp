

#include "MusicStreamer.h"

MusicStreamer::MusicStreamer(std::unordered_set<StreamerClient *> *clients)
        : clients(clients) {}

void MusicStreamer::init() {

}

MusicStreamer::~MusicStreamer() {}
