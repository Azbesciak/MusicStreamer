#ifndef MUSICSTREAMER_SOUNDCHUNK_H
#define MUSICSTREAMER_SOUNDCHUNK_H


class SoundChunk {

private:

    char* content;
    int bytes;

public:

    SoundChunk(char* content, int bytes);

    char* getContent();
    int getBytes();

    ~SoundChunk();
};


#endif //MUSICSTREAMER_SOUNDCHUNK_H
