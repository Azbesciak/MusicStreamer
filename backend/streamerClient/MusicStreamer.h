

#ifndef MUSICSTREAMER_MUSICSTREAMER_H
#define MUSICSTREAMER_MUSICSTREAMER_H


#include <unordered_set>
#include "StreamerClient.h"

class MusicStreamer {
private:
    std::unordered_set<StreamerClient*>* clients;
public:
    explicit MusicStreamer(std::unordered_set<StreamerClient*>* clients);
    void init();
    ~MusicStreamer();
};


#endif //MUSICSTREAMER_MUSICSTREAMER_H
