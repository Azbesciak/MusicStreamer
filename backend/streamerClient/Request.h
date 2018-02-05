
#ifndef MUSICSTREAMER_REQUEST_H
#define MUSICSTREAMER_REQUEST_H

#include <string>

#include <utility/json.hpp>
using json = nlohmann::json;

class Request {
    std::string method;
    json request;
    std::string original;
    bool correct = true;
public:
    explicit Request(char * request);
    int getInt(const std::string &name);
    std::string getStr(const std::string & name);
    std::string getMethod();
    bool has(const std::string &name);
    std::string serialize();
    bool isCorrect();
};


#endif //MUSICSTREAMER_REQUEST_H
