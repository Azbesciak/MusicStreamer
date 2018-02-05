
#ifndef MUSICSTREAMER_REQUESTRESPONSESERVER_H
#define MUSICSTREAMER_REQUESTRESPONSESERVER_H
#include "AbstractServer.h"
#include "SocketFactory.h"

class RequestResponseServer: AbstractServer {
public:
    RequestResponseServer(const string &host, int port, Container *container, atomic<bool> *isRunning);
    ~RequestResponseServer();

private:
    int createSocket() override;

};


#endif //MUSICSTREAMER_REQUESTRESPONSESERVER_H
