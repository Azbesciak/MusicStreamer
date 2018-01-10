#include "Message.h"

Message::Message(vector<StreamerClient *> receivers, const string &message)
    :content(message), receivers(std::move(receivers)) {}

vector<StreamerClient*> Message::getReceivers() const {
    return receivers;
}

string Message::getContent() const {
    return content;
}
