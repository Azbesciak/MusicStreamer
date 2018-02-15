

#include <vector>
#include <unordered_set>

using namespace std;

#ifndef MUSICSTREAMER_MESSAGE_H
#define MUSICSTREAMER_MESSAGE_H

class StreamerClient;

class Message {
    unordered_set<StreamerClient*> receivers;
    string content;
public:
    Message(const unordered_set<StreamerClient*> &receivers, const string &message);
    unordered_set<StreamerClient*> getReceivers() const;
    string getContent() const;
};
#endif //MUSICSTREAMER_MESSAGE_H
