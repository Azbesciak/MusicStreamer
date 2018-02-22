

#include "TrackHeader.h"
#include "MusicTrack.h"


TrackHeader::TrackHeader() {
}


TrackHeader::TrackHeader(const char* headerBytes) {

    sampleRate = parseSampleRate(headerBytes);
    bitsPerSample = parseBitsPerSample(headerBytes);
    channels = parseChannels(headerBytes);
    byteRate = parseByteRate(headerBytes);
    soundSize = parseSoundSize(headerBytes);
}


int TrackHeader::parseHeaderNumber(const char* header, int start, int bytes) {

    int number = 0;

    for (int i = start; i < start + bytes; ++i) {

        int byte = header[i];

        if (byte < 0)
            byte += 256;

        byte <<= ((i - start) * 8);
        number += byte;
    }

    return number;
}


int TrackHeader::parseSampleRate(const char* header) {
    return parseHeaderNumber(header, 24, 4);
}


int TrackHeader::parseBitsPerSample(const char* header) {
    return parseHeaderNumber(header, 34, 2);
}


int TrackHeader::parseChannels(const char* header) {
    return parseHeaderNumber(header, 22, 2);
}


int TrackHeader::parseByteRate(const char* header) {
    return parseHeaderNumber(header, 28, 4);
}


int TrackHeader::parseSoundSize(const char* header) {
    return parseHeaderNumber(header, 40, 4);
}


bool TrackHeader::isSampleRateValid(int sampleRate) {
    return (7500 < sampleRate && sampleRate < 100000);
}


bool TrackHeader::isBitsPerSampleValid(int bitsPerSample) {
    return (bitsPerSample > 0);
}


bool TrackHeader::isChannelsValid(int channels) {
    return (channels > 0);
}


bool TrackHeader::isValid() {
    return true;
}


ClientResponse TrackHeader::toJsonMessage() {

    ClientResponse jsonHeader;

    jsonHeader.addToBody("sampleRate", sampleRate);
    jsonHeader.addToBody("bitsPerSample", bitsPerSample);
    jsonHeader.addToBody("channels", channels);

    jsonHeader.setStatus(200);

    return jsonHeader;
}

int TrackHeader::getSize() {
    return MusicTrack::WAV_HEADER_SIZE;
}

int TrackHeader::getSampleRate() {
    return sampleRate;
}

int TrackHeader::getBitsPerSample() {
    return bitsPerSample;
}

int TrackHeader::getChannels() {
    return channels;
}

int TrackHeader::getByteRate() {
    return byteRate;
}

int TrackHeader::getSoundSize() {
    return soundSize;
}
