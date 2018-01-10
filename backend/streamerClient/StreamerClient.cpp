
#include <unistd.h>
#include "../utility/json.hpp"
#include "StreamerClient.h"

using json = nlohmann::json;
using namespace std;

StreamerClient::StreamerClient(int socketDescriptor) :
        socketDescriptor(socketDescriptor){}

string StreamerClient::getName() const {
    return name;
}

StreamerClient::~StreamerClient() {
    close(socketDescriptor);
}

ssize_t StreamerClient::sendMessage(const string &message) {
    return write(socketDescriptor, message.c_str(), message.size());
}

void StreamerClient::setName(const string &name) {
    this->name = name;
}
