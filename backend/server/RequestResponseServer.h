
#ifndef MUSICSTREAMER_REQUESTRESPONSESERVER_H
#define MUSICSTREAMER_REQUESTRESPONSESERVER_H

#include "TcpServer.h"

class RequestResponseServer : public TcpServer {
public:
    RequestResponseServer(const string &host, int port, ServerManager *manager);

};


#endif //MUSICSTREAMER_REQUESTRESPONSESERVER_H
