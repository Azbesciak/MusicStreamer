

#include <vector>
#include <unordered_set>
#include "../streamerClient/StreamerClient.h"

using namespace std;

#ifndef MUSICSTREAMER_MESSAGE_H
#define MUSICSTREAMER_MESSAGE_H
class Message {
    unordered_set<StreamerClient*> receivers;
    string content;
public:
    Message(unordered_set<StreamerClient*> receivers, const string &message);
    unordered_set<StreamerClient*> getReceivers() const;
    string getContent() const;
};
#endif //MUSICSTREAMER_MESSAGE_H
