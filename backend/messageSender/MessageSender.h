

#include <thread>
#include <queue>
#include "../utility/synch.h"
#include "Message.h"

using namespace std;

#ifndef MUSICSTREAMER_MESSAGESENDER_H
#define MUSICSTREAMER_MESSAGESENDER_H

class StreamerClient;
class Room;

class MessageSender {
    thread* broadcastThread;
    queue<Message> messages;
    recursive_mutex mesMut;
public:
    MessageSender();
    ~MessageSender();
    void sendMessage(Message &message);
};

#endif //MUSICSTREAMER_MESSAGESENDER_H
