
#include <iostream>
#include "Request.h"

static const char *const METHOD_KEY = "method";

Request::Request(char *message): Request(std::string(message)) {}

Request::Request(const std::string &message, bool correct) {
    original = message;
    if (correct) {
        try {
            request = json::parse(message);
            method = request.at(METHOD_KEY).get<std::string>();
        } catch (json::exception &e) {
            std::cerr<< "ERROR parsing message:" << e.what() <<std::endl;
            this->correct = false;
        }
    } else {
        this->correct = false;
    }
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

std::string Request::serialize() {
    return std::__cxx11::string();
}

bool Request::isCorrect() {
    return correct;
}

