

#ifndef MUSICSTREAMER_REQUESTPROCESSOR_H
#define MUSICSTREAMER_REQUESTPROCESSOR_H


#include "ClientResponse.h"
#include "Request.h"

class RequestProcessor {
    virtual ClientResponse onNewRequest(Request * request, const string &method, ClientResponse *&response) = 0;

public:
    virtual ClientResponse onNewRequest(Request * request);
    virtual ssize_t respond(int socket, const string & message);

};


#endif //MUSICSTREAMER_REQUESTPROCESSOR_H
