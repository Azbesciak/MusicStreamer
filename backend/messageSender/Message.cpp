#include "Message.h"

Message::Message(const unordered_set<StreamerClient *> &receivers, const string &message)
    :content(message), receivers(std::move(receivers)) {}

unordered_set<StreamerClient*> Message::getReceivers() const {
    return receivers;
}

string Message::getContent() const {
    return content;
}
