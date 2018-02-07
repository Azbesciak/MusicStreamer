#include "WavSignalStreamer.h"

WavSignalStreamer::WavSignalStreamer(const string &host, int port, ServerManager *manager, const string &serverName)
        : UdpServer(host, port, manager, serverName) {}
