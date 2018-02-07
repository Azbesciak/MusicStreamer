#ifndef MUSICSTREAMER_WAVHEADERSTREAMER_H
#define MUSICSTREAMER_WAVHEADERSTREAMER_H


#include <server/TcpServer.h>

class WavHeaderStreamer : public TcpServer {

public:

    WavHeaderStreamer(const string &host, int port, ServerManager *manager, const string &serverName);

};


#endif //MUSICSTREAMER_WAVHEADERSTREAMER_H
