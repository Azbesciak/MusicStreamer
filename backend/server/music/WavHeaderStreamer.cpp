#include "WavHeaderStreamer.h"

WavHeaderStreamer::WavHeaderStreamer(const string &host, int port, ServerManager *manager, const string &serverName)
        : TcpServer(host, port, manager, serverName) {}
