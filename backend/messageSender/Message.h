

#include <vector>
#include "../streamerClient/StreamerClient.h"

using namespace std;

#ifndef MUSICSTREAMER_MESSAGE_H
#define MUSICSTREAMER_MESSAGE_H
class Message {
    vector<StreamerClient*> receivers;
    string content;
public:
    Message(vector<StreamerClient*> receivers, const string &message);
    vector<StreamerClient*> getReceivers() const;
    string getContent() const;
};
#endif //MUSICSTREAMER_MESSAGE_H
