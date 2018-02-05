
#include "Request.h"

static const char *const METHOD_KEY = "method";

Request::Request(char *message) {
    request = json::parse(message);
    method = request.at(METHOD_KEY).get<std::string>();
}

std::string Request::getStr(const std::string &name) {
    return request.at(name).get<std::string>();
}

int Request::getInt(const std::string &name) {
    return request.at(name).get<int>();
}

std::string Request::getMethod() {
    return method;
}

bool Request::has(const std::string &name) {
    return request.find(name) != request.end();
}
