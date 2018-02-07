#ifndef MUSICSTREAMER_WAVSIGNALSTREAMER_H
#define MUSICSTREAMER_WAVSIGNALSTREAMER_H


#include <server/UdpServer.h>

class WavSignalStreamer : public UdpServer {

public:

    WavSignalStreamer(const string &host, int port, ServerManager *manager, const string &serverName);
};


#endif //MUSICSTREAMER_WAVSIGNALSTREAMER_H
