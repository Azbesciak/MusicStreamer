#include "ClientResponse.h"


string ClientResponse::serialize() {
    string resp ="{\"status\":" ;
    resp +=  to_string(status);
    resp += ",\"body\":";
    resp += body.dump() + "}";
    return resp;
}

void ClientResponse::setBody(json body) {
    this->body = std::move(body);
}

void ClientResponse::setStatus(int status) {
    this->status = status;
}

void ClientResponse::setError(int code, string message) {
    setStatus(code);
    auto errorBody = json();
    errorBody["error"] = message;
    setBody(errorBody);
}

ClientResponse::ClientResponse(): status(-1), body(json()) {}

void ClientResponse::addToBody(string key, vector<string> value) {
    body[key] = json(value);
}

void ClientResponse::fillOkResultIfNotSet() {
    if (status < 0) {
        setStatus(200);
    }
    if (body.empty()) {
        body = json("OK");
    }
}

ClientResponse::ClientResponse(int status, string message)
        : status(status), body(json(message)) {}

ClientResponse ClientResponse::error(int status, const string &message) {
    ClientResponse resp;
    resp.setError(status, message);
    return resp;
}


