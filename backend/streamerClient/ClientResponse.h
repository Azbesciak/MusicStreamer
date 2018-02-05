#include "../utility/json.hpp"

using json = nlohmann::json;
using namespace std;
#ifndef MUSICSTREAMER_CLIENTRESPONSE_H
#define MUSICSTREAMER_CLIENTRESPONSE_H


class ClientResponse {
    int status;
    json body;
public:
    ClientResponse();
    ClientResponse(int status, string message);
    void setStatus(int status);
    void setBody(json body);
    string serialize();
    void addToBody(string key, vector<string> value);
    void addToBody(string key, const string &value);
    void setError(int code, string message);
    void fillOkResultIfNotSet();
    static ClientResponse error(int status, const string &message);
    bool isError();

};


#endif //MUSICSTREAMER_CLIENTRESPONSE_H
