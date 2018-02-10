#include "../utility/json.hpp"
#include "Request.h"

using json = nlohmann::json;
using namespace std;
#ifndef MUSICSTREAMER_CLIENTRESPONSE_H
#define MUSICSTREAMER_CLIENTRESPONSE_H


class ClientResponse {
    int status;
    bool hasId = false;
    string id;
    json body;
public:
    ClientResponse();
    ClientResponse(int status, const string &message);
    void setStatus(int status);
    void setBody(json body);
    string serialize();
    void addToBody(const string &key, vector<string> value);
    void addToBody(const string &key, const string &value);
    void addToBody(const string &key, int value);
    void setError(int code, string message);
    void fillOkResultIfNotSet();

    static ClientResponse ok();
    static ClientResponse error(int status, const string &message);

    bool isError();
    void asUnknownResponse();
    void addIdFromRequestIfPresent(Request *request);

};


#endif //MUSICSTREAMER_CLIENTRESPONSE_H
