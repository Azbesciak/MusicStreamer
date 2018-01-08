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
    void setStatus(int status);
    void setBody(json body);
    string serialize();
    void addToBody(string key, vector<string> value);
    void setError(int code, string message);
    void fillOkResultIfNotSet();

};


#endif //MUSICSTREAMER_CLIENTRESPONSE_H
