#include "SoundChunk.h"

SoundChunk::SoundChunk(char* content, int bytes) {

    this->content = content;
    this->bytes = bytes;
}


char* SoundChunk::getContent() {
    return content;
}


int SoundChunk::getBytes() {
    return bytes;
}


SoundChunk::~SoundChunk() {
    delete[] content;
}
