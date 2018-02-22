
#ifndef MUSICSTREAMER_TRACKHEADER_H
#define MUSICSTREAMER_TRACKHEADER_H


#include <streamerClient/ClientResponse.h>

class TrackHeader {

private:

    int sampleRate;
    int bitsPerSample;
    int channels;
    int byteRate;
    int soundSize;

    int parseHeaderNumber(const char* header, int start, int bytes);

    int parseSampleRate(const char* header);
    int parseBitsPerSample(const char* header);
    int parseChannels(const char* header);
    int parseByteRate(const char* header);
    int parseSoundSize(const char* header);


    static bool isSampleRateValid(int sampleRate);
    static bool isBitsPerSampleValid(int bitsPerSample);
    static bool isChannelsValid(int channels);

public:

    TrackHeader();
    explicit TrackHeader(const char* headerBytes);

    int getSize();
    int getSampleRate();
    int getBitsPerSample();
    int getChannels();
    int getByteRate();
    int getSoundSize();

    bool isValid();

    ClientResponse toJsonMessage();
};


#endif //MUSICSTREAMER_TRACKHEADER_H
